/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "AWSPubsubImpl.h"
#include "ICloudConnector.h"
#include "Utilities.h"
#include "ClientFactory.h"

#include "Async/Async.h"
#include "Internationalization/Regex.h"
#include "HAL/Thread.h"
#include "Engine/Engine.h" // to get viewport
#include "Engine/GameViewportClient.h" // to get viewport

 // AWS SDK
#include "Windows/PreWindowsApi.h"
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/client/AWSError.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/utils/DateTime.h>

#include <aws/sqs/SQSClient.h>
#include <aws/sqs/SQSRequest.h>
#include <aws/sqs/model/QueueAttributeName.h>
#include <aws/sqs/model/CreateQueueRequest.h>
#include <aws/sqs/model/CreateQueueResult.h>
#include <aws/sqs/model/GetQueueAttributesRequest.h>
#include <aws/sqs/model/GetQueueAttributesResult.h>
#include <aws/sqs/model/DeleteQueueRequest.h>
#include <aws/sqs/model/ReceiveMessageRequest.h>
#include <aws/sqs/model/ReceiveMessageResult.h>
#include <aws/sqs/model/DeleteMessageRequest.h>
#include <aws/sqs/model/ChangeMessageVisibilityRequest.h>
#include <aws/sns/SNSClient.h>
#include <aws/sns/SNSRequest.h>
#include <aws/sns/model/CreateTopicRequest.h>
#include <aws/sns/model/CreateTopicResult.h>
#include <aws/sns/model/SubscribeRequest.h>
#include <aws/sns/model/SubscribeResult.h>
#include "Windows/PostWindowsApi.h"

#include <atomic>


/* Those objects are supposedly threadsafe and tests have shown that they really appear to be so
 * it seems to be safe to concurrently access this from multiple threads at once
 */
static Aws::UniquePtr<Aws::SNS::SNSClient> s_sns_client;
static Aws::UniquePtr<Aws::SQS::SQSClient> s_sqs_client;


using namespace Aws::SNS::Model;
using namespace Aws::SQS::Model;

AWSPubsubImpl::AWSPubsubImpl(const bool n_handle_in_game_thread)
		: m_handle_in_game_thread{ n_handle_in_game_thread } {

}

AWSPubsubImpl::~AWSPubsubImpl() noexcept {

	shutdown();
}

void AWSPubsubImpl::shutdown() noexcept {

	m_shut_down.store(true);
	shutdown_runners(nullptr);
}

void AWSPubsubImpl::shutdown_runners(FViewport *n_viewport) noexcept {

	try {
		// user may have not unsubscribed
		TArray<FSubscription> remaining_subs;
		remaining_subs.Reserve(m_subscriptions.Num());
		{
			FScopeLock slock(&m_subscriptions_mutex);
			for (const TPair<FSubscription, TUniquePtr<SQSSubscription> > &i : m_subscriptions) {
				remaining_subs.Add(i.Key);
			}
		}

		for (FSubscription &s : remaining_subs) {
			unsubscribe(MoveTemp(s));
		}

		// de-register from the Alt+F4 delegate
		if (m_emergency_shutdown_handle.IsValid() && GEngine && GEngine->GameViewport) {

			GEngine->GameViewport->OnCloseRequested().Remove(m_emergency_shutdown_handle);
		}

	} catch (const std::exception &sex) {
		UE_LOG(LogCloudConnector, Error, TEXT("Unexpected exception tearing down SQS: %s"), UTF8_TO_TCHAR(sex.what()));
	}
};

class SQSSubscription {

	public:
		SQSSubscription(const FSubscription &n_subscription, const bool n_handle_on_game_thread, const FPubsubMessageReceived n_handler)
				: m_subscription{ n_subscription }
				, m_handle_on_game_thread{ n_handle_on_game_thread }
				, m_handler{ n_handler } {

			// Sanity please
			checkf(m_handler.IsBound(), TEXT("Handler for SQS must be bound"));

			// And fire up a thread to run while we're gone
			m_runner = MakeUnique<FThread>(TEXT("AWS SQS Subscription Runner"), [this] { this->run(); });
		}

		void run() noexcept {

			// We have out own clients
			m_sqs = aws_client_factory<Aws::SQS::SQSClient>::create();
			m_sns = aws_client_factory<Aws::SNS::SNSClient>::create();

			if (!create_queue()) {
				UE_LOG(LogCloudConnector, Warning, TEXT("Subscription runner existing due to Q setup error"));
				return;
			}

			if (!create_topic()) {
				UE_LOG(LogCloudConnector, Warning, TEXT("Subscription runner existing due to setup error"));
				return;
			}

			if (!subscribe_to_q()) {
				UE_LOG(LogCloudConnector, Warning, TEXT("Subscription runner existing due to failure in subscription process"));
				return;
			}

			// Loop and receive messages, re-use request object
			// I've had horrendous problems tearing this thread down.
			// When this object lives on the stack it will crash in the d'tor
			// for unknown reasons. So I have to create it dynamic and leak it intentionally.
			//
			Aws::UniquePtr<ReceiveMessageRequest> rm_req = Aws::MakeUnique<ReceiveMessageRequest>("SQSREQ");
			rm_req->SetQueueUrl(m_queue_url);
			rm_req->SetMaxNumberOfMessages(1);

			// This is not a timeout per se but long polling, which means the call will return
			// After this many seconds even if there are no messages, which is not an error.
			// See https://docs.aws.amazon.com/AWSSimpleQueueService/latest/SQSDeveloperGuide/sqs-short-and-long-polling.html#sqs-long-polling
			//
			// I am hard-choosing 4 here as values of 5 and above caused weird errors in testing.
			//
			rm_req->SetWaitTimeSeconds(4);

			// Request some additional info with each message
			rm_req->SetAttributeNames({

				// For X-Ray tracing, I need an AWSTraceHeader, which in the C++ SDK 
				// seems to be not in the enum like the others. To have it included in the response,
				// only All will do the trick.
				QueueAttributeName::All,

				QueueAttributeName::SentTimestamp,             // to calculate age
				QueueAttributeName::ApproximateReceiveCount    // to see how often that message has been received (approximation)
			});

			// This is how it's supposed to work but it doesn't. Hence the 'All' above
			rm_req->SetMessageAttributeNames({
				MessageSystemAttributeNameMapper::GetNameForMessageSystemAttributeName(MessageSystemAttributeName::AWSTraceHeader)
			});

			while (!m_interrupted.load()) 	{
		
				ReceiveMessageOutcome rm_out = m_sqs->ReceiveMessage(*rm_req);
				if (!rm_out.IsSuccess()) {
					UE_LOG(LogCloudConnector, Warning, TEXT("Failed to retrieve message from queue '%s': '%s'"),
							UTF8_TO_TCHAR(m_queue_url.c_str()), UTF8_TO_TCHAR(rm_out.GetError().GetMessage().c_str()));
					continue;
				}

				if (rm_out.GetResult().GetMessages().empty()) {
					UE_LOG(LogCloudConnector, Display, TEXT("Long polling from queue '%s' returned with a timeout of 4s, no new messages"),
							UTF8_TO_TCHAR(m_queue_url.c_str()));
					continue;
				}

				UE_LOG(LogCloudConnector, Verbose, TEXT("Long polling with a timeout of 4s returned from queue '%s', %i messages"),
						UTF8_TO_TCHAR(m_queue_url.c_str()), rm_out.GetResult().GetMessages().size());

				// Process the message and call handler
				process_message(rm_out.GetResult().GetMessages()[0]);

				// Rinse, repeat until interrupted
			}

			rm_req.release(); // yes, sad.
							  // Leaking this object is the only way I found around
							  // https://github.com/MrMoose/CloudConnector/issues/3
		}

		/// Stops the runnable object from foreign thread
		void interrupt() {

			m_interrupted.store(true);
		}

		/// call this on game thread
		void join() {
			
			// There's a bug here. 
			m_runner->Join();
			m_runner.Reset();
		}

	private:

		void receive_message_handler(const Aws::SQS::SQSClient *n_client, const ReceiveMessageRequest &n_request,
				const ReceiveMessageOutcome &n_outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext> &n_context) {
	
			if (!n_outcome.IsSuccess()) {
				UE_LOG(LogCloudConnector, Warning, TEXT("Failed to retrieve message from queue '%s': '%s'"),
						UTF8_TO_TCHAR(m_queue_url.c_str()), UTF8_TO_TCHAR(n_outcome.GetError().GetMessage().c_str()));
				return;
			}

			if (n_outcome.GetResult().GetMessages().empty()) {
				UE_LOG(LogCloudConnector, Display, TEXT("Long polling from queue '%s' returned with a timeout of 4s, no new messages"),
					UTF8_TO_TCHAR(m_queue_url.c_str()));
				return;
			}

			UE_LOG(LogCloudConnector, Verbose, TEXT("Long polling with a timeout of 4s returned from queue '%s', %i messages"),
					UTF8_TO_TCHAR(m_queue_url.c_str()), n_outcome.GetResult().GetMessages().size());

			// Process the message and call handler
			process_message(n_outcome.GetResult().GetMessages()[0]);
		}


		void process_message(const Message &n_message) const noexcept {

			UE_LOG(LogCloudConnector, Display, TEXT("processing message '%s'"), UTF8_TO_TCHAR(n_message.GetMessageId().c_str()));

			// Determine message age
			const int64_t current_epoch = Aws::Utils::DateTime::CurrentTimeMillis();
			int64_t sent_epoch = 0;

			const Aws::Map<MessageSystemAttributeName, Aws::String> &attributes = n_message.GetAttributes();

			Aws::Map<MessageSystemAttributeName, Aws::String>::const_iterator i =
					attributes.find(MessageSystemAttributeName::SentTimestamp);
			if (i != attributes.cend()) 	{
				sent_epoch = std::stoull(i->second.c_str());
			};
			const int64_t age = current_epoch - sent_epoch;

			// Get ApproximateReceiveCount
			int received = 0;
			i = attributes.find(MessageSystemAttributeName::ApproximateReceiveCount);
			if (i != attributes.cend()) {
				received = std::atoi(i->second.c_str());
			};

			// Now construct a message which will be given into the handler
			FPubsubMessage msg;
			msg.m_aws_sqs_message_id = UTF8_TO_TCHAR(n_message.GetMessageId().c_str());
			msg.m_aws_sqs_receipt = UTF8_TO_TCHAR(n_message.GetReceiptHandle().c_str());
			msg.m_message_age = current_epoch - sent_epoch;
			msg.m_receive_count = received;
			msg.m_body = UTF8_TO_TCHAR(n_message.GetBody().c_str());
			
			// This promise will be fulfilled by the delegate implementation
			const PubsubReturnPromisePtr rp = MakeShared<PubsubReturnPromise, ESPMode::ThreadSafe>();
			TFuture<bool> return_future = rp->GetFuture();

			// Call the delegate on the game thread
			if (m_handle_on_game_thread) {
				Async(EAsyncExecution::TaskGraphMainThread, [&msg, this, &rp] {
					this->m_handler.ExecuteIfBound(msg, rp);
				});
			} else {
				this->m_handler.ExecuteIfBound(msg, rp);
			}

			// Now we wait for the delegate impl to call SetValue() on the promise.
			// This might take forever if the implementation is not careful.
			// It might be worthwhile to include a timeout but I want to discuss this first.
			return_future.Wait();
			if (return_future.Get()) {
				delete_message(n_message);
			} else {
				UE_LOG(LogCloudConnector, Display, TEXT("Msg handler returned false, not deleting message '%s'"), *msg.m_aws_sqs_message_id);
			}
		}

		void delete_message(const Message &n_message) const noexcept {

			DeleteMessageRequest request;
			request.SetQueueUrl(m_queue_url);
			request.SetReceiptHandle(n_message.GetReceiptHandle());

			// issue the delete request
			const DeleteMessageOutcome outcome = m_sqs->DeleteMessage(request);

			if (outcome.IsSuccess()) 	{
				UE_LOG(LogCloudConnector, Display, TEXT("Deleted message '%s'"), UTF8_TO_TCHAR(n_message.GetMessageId().c_str()));
			} 	else 	{
				UE_LOG(LogCloudConnector, Error, TEXT("Deletion of message '%s' failed: %s"), UTF8_TO_TCHAR(n_message.GetMessageId().c_str()), UTF8_TO_TCHAR(outcome.GetError().GetMessage().c_str()));
			}
		}

		bool create_queue() {

			CreateQueueRequest cqr;
			cqr.SetQueueName(TCHAR_TO_UTF8(*m_subscription.Id));
			cqr.SetTags({ { "created_by", "CloudConnector" } });
			cqr.SetAttributes({
				//{ QueueAttributeName::FifoQueue,                     "true" },
				{ QueueAttributeName::DelaySeconds,                  "0" },
				{ QueueAttributeName::VisibilityTimeout,             "60" },
				{ QueueAttributeName::ReceiveMessageWaitTimeSeconds, "4" }
				});

			const CreateQueueOutcome cqoc = s_sqs_client->CreateQueue(cqr);

			if (!cqoc.IsSuccess()) {
				UE_LOG(LogCloudConnector, Warning, TEXT("AWS Pubsub failed to create SQS for SNS subscription: %s"),
						UTF8_TO_TCHAR(cqoc.GetError().GetMessage().c_str()));
				return false;
			} else {
				m_queue_url = cqoc.GetResult().GetQueueUrl();
				UE_LOG(LogCloudConnector, Display, TEXT("Created temporary SQS for SNS subscription with url: %s"), UTF8_TO_TCHAR(m_queue_url.c_str()));
			}

			GetQueueAttributesRequest gqar;
			gqar.SetQueueUrl(cqoc.GetResult().GetQueueUrl());
			gqar.SetAttributeNames({ QueueAttributeName::QueueArn });

			// Now that we have a Q, hook it up to that subscription
			// In order to do that we need the arn of the Q we have just created which weirdly
			// doesn't seem to be in the response. So we need to get it.
			const GetQueueAttributesOutcome gqaoc = m_sqs->GetQueueAttributes(gqar);
			if (!gqaoc.IsSuccess()) {
				UE_LOG(LogCloudConnector, Warning, TEXT("AWS didn't get us Queue attributes: %s"),
						UTF8_TO_TCHAR(gqaoc.GetError().GetMessage().c_str()));
				delete_queue();
				return false;
			} else {
				const auto i = gqaoc.GetResult().GetAttributes().find(QueueAttributeName::QueueArn);
				if (i == gqaoc.GetResult().GetAttributes().end()) {
					UE_LOG(LogCloudConnector, Warning, TEXT("AWS didn't give us Queue attributes: %s"),
							UTF8_TO_TCHAR(gqaoc.GetError().GetMessage().c_str()));
					delete_queue();
					return false;
				} else {
					m_queue_arn = i->second;
					UE_LOG(LogCloudConnector, Display, TEXT("Got ARN of temporary Q: %s"), UTF8_TO_TCHAR(m_queue_arn.c_str()));
				}
			}
			return true;
		}

		bool create_topic() {

			// Now... I am assuming the user created the topic already as part of IaC
			// So I really only need the existing Topic's ARN. This doesn't seem to be 
			// possible though.
			// Those guys here: https://stackoverflow.com/questions/36721014/aws-sns-how-to-get-topic-arn-by-topic-name
			// say it's better to CreateTopic which will not create it if it already 
			// exists but return the ARN anyway

			CreateTopicRequest ctar;
			ctar.SetName(TCHAR_TO_UTF8(*m_subscription.Topic));
			Aws::SNS::Model::Tag creation_tag;
			creation_tag.SetKey("created_by");
			creation_tag.SetValue("CloudConnector");
			ctar.SetTags({ creation_tag });
			// I roll with defaults here until I know which values make sense

			const CreateTopicOutcome ctoc = m_sns->CreateTopic(ctar);
			if (!ctoc.IsSuccess()) {

				// #moep debug this error type and check how SNS reacts on already existing topic

				UE_LOG(LogCloudConnector, Warning, TEXT("AWS refused to create topic: %s"),
							UTF8_TO_TCHAR(ctoc.GetError().GetMessage().c_str()));

				return false;
			} else {
				m_topic_arn = ctoc.GetResult().GetTopicArn();
				UE_LOG(LogCloudConnector, Display, TEXT("Got Topic ARN: %s"), UTF8_TO_TCHAR(m_topic_arn.c_str()));
			}
			return true;
		}

		// Hook up our temporary Q twe created for us
		bool subscribe_to_q() const {

			SubscribeRequest s_req;
			s_req.SetTopicArn(m_topic_arn);
			s_req.SetProtocol("sqs");
			s_req.SetEndpoint(m_queue_arn);

			const SubscribeOutcome soc = m_sns->Subscribe(s_req);

			if (!soc.IsSuccess()) {
				UE_LOG(LogCloudConnector, Warning, TEXT("AWS Pubsub failed to subscribe temporary Q to topic '%s': %s"),
						*m_subscription.Topic, UTF8_TO_TCHAR(soc.GetError().GetMessage().c_str()));
				return false;
			} else {
				UE_LOG(LogCloudConnector, Display, TEXT("Subscribed temporary Q to topic '%s'"), *m_subscription.Topic);
				return true;
			}
		}

		void delete_queue() const {

			DeleteQueueRequest dqr;
			dqr.SetQueueUrl(m_queue_url);
			const DeleteQueueOutcome dqoc = s_sqs_client->DeleteQueue(dqr);

			if (!dqoc.IsSuccess()) {
				UE_LOG(LogCloudConnector, Warning, TEXT("AWS Pubsub failed to delete temporary SQS Q '%s': %s"),
						UTF8_TO_TCHAR(m_queue_url.c_str()), UTF8_TO_TCHAR(dqoc.GetError().GetMessage().c_str()));
			} else {
				UE_LOG(LogCloudConnector, Display, TEXT("Deleted temporary SQS Q '%s'"), UTF8_TO_TCHAR(m_queue_url.c_str()));
			}
		}

		FSubscription                        m_subscription;
		Aws::String                          m_topic_arn;
		Aws::String                          m_queue_url;
		Aws::String                          m_queue_arn;

		const bool                           m_handle_on_game_thread;
		const FPubsubMessageReceived         m_handler;
		std::atomic<bool>                    m_interrupted = false;
		ReceiveMessageOutcomeCallable        m_future_out;
		Aws::UniquePtr<Aws::SQS::SQSClient>  m_sqs;
		Aws::UniquePtr<Aws::SNS::SNSClient>  m_sns;
		TUniquePtr<FThread>                  m_runner;
};


bool AWSPubsubImpl::subscribe(const FString &n_topic, FSubscription &n_subscription, const FPubsubMessageReceived n_handler) {

	using namespace Aws::SQS::Model::QueueAttributeNameMapper;

	if (m_shut_down) {
		UE_LOG(LogCloudConnector, Display, TEXT("AWS Pubsub Object is being shut down. Ignore subscribe cmd"));
		return false;
	}

	// To subscribe to the topic we need to create an SQS queue to receive messages.
	// This could be done by the user but in order to mimic the Google impl I do it on the fly
	// and remove them afterwards. This might result in leftover Queues so, we may have to reconsider.

	const FString instance_id = get_aws_instance_id();
	n_subscription.Topic = n_topic;
	n_subscription.Id = FString::Printf(TEXT("CCQ-%s-%s"), *instance_id, *n_subscription.Topic);

	// In here it is enough to remember the id aka Queue URL
	n_subscription.Id = n_topic;
	n_subscription.Topic = n_topic;

	{
		FScopeLock slock(&m_subscriptions_mutex);
		if (m_subscriptions.Contains(n_subscription)) {
			UE_LOG(LogCloudConnector, Error, TEXT("Already subscribed to '%s'"), *n_topic);
			return false;
		}
	}

	TUniquePtr<SQSSubscription> runner = MakeUnique<SQSSubscription>(n_subscription, m_handle_in_game_thread, n_handler);

	// hooko up to Unreal's Alt+F4 catch-all delegate to shut down SQS gracefully in this case
	if (!m_emergency_shutdown_handle.IsValid() && GEngine && GEngine->GameViewport) {

		m_emergency_shutdown_handle = GEngine->GameViewport->OnCloseRequested().AddRaw(this, &AWSPubsubImpl::shutdown_runners);
	}

	// Take ownership over both by inserting it into our subscription map
	FScopeLock slock(&m_subscriptions_mutex);
	m_subscriptions.Emplace(n_subscription, MoveTemp(runner));

	return true;
}

bool AWSPubsubImpl::unsubscribe(FSubscription &&n_subscription) {
	
	if (m_shut_down) {
		UE_LOG(LogCloudConnector, Display, TEXT("AWS Pubsub Object is being shut down. Ignore unsubscribe cmd"));
		return false;
	}

	// Unsubscribing here simply means to stop the pull
	
	// Locate the subscriber
	TUniquePtr<SQSSubscription> *r = nullptr;
	
	{
		FScopeLock slock(&m_subscriptions_mutex);
		r = m_subscriptions.Find(n_subscription);
		if (!r) {
			UE_LOG(LogCloudConnector, Error, TEXT("Cannot unsubscribe from '%s', internal data missing"), *n_subscription.Id);
			return false;
		}
	
	
		// First stop the thread.
		// Killing it this way waits for the thread to join.
		checkf(r, TEXT("internal data corruption"));
		(*r)->interrupt();
		(*r)->join();

		// And delete all the data
		//FScopeLock slock(&m_subscriptions_mutex);
		m_subscriptions.Remove(n_subscription);
	}
	// And we're done
	UE_LOG(LogCloudConnector, Display, TEXT("Unsubscribed from '%s'"), *n_subscription.Id);
	return true;
}

bool AWSPubsubImpl::publish(const FString &n_topic, const FString &n_message, FPubsubMessagePublished &&n_handler) {

	return false;
}