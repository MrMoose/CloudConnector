/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "AWSPubsubImpl.h"
#include "ICloudConnector.h"
#include "Utilities.h"

#include "Async/Async.h"
#include "Internationalization/Regex.h"

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
#include <aws/sqs/model/ReceiveMessageRequest.h>
#include <aws/sqs/model/ReceiveMessageResult.h>
#include <aws/sqs/model/DeleteMessageRequest.h>
#include <aws/sqs/model/ChangeMessageVisibilityRequest.h>
#include "Windows/PostWindowsApi.h"

FCriticalSection AWSPubsubImpl::s_subscriptions_mutex;

using namespace Aws::SQS::Model;

AWSPubsubImpl::AWSPubsubImpl(const bool n_handle_in_game_thread)
		: m_handle_in_game_thread{ n_handle_in_game_thread } {

}

AWSPubsubImpl::~AWSPubsubImpl() {

	try {
		// user may have not unsubscribed
		TArray<FSubscription> remaining_subs;
		remaining_subs.Reserve(m_subscriptions.Num());
		{
			FScopeLock slock(&s_subscriptions_mutex);
			for (const TPair<FSubscription, SQSSubscriptionTuple> &i : m_subscriptions) {
				remaining_subs.Add(i.Key);
			}
		}

		for (FSubscription &s : remaining_subs) {
			unsubscribe(MoveTemp(s));
		}
	} catch (const std::exception &sex) {
		UE_LOG(LogCloudConnector, Error, TEXT("Unexpected exception tearing down SQS: %s"), UTF8_TO_TCHAR(sex.what()));
	}
}

class SQSRunner : public FRunnable {

	public:
		SQSRunner(const FString &n_queue_url, const bool n_handle_on_game_thread, const FPubsubMessageReceived n_handler)
				: m_queue_url{ TCHAR_TO_UTF8(*n_queue_url) }
				, m_handle_on_game_thread{ n_handle_on_game_thread }
				, m_handler{ n_handler } {
		}

		bool Init() override {

			Aws::Client::ClientConfiguration client_config;
			client_config.enableEndpointDiscovery = use_endpoint_discovery();
			const FString sqs_endpoint = readenv(TEXT("CLOUDCONNECTOR_SQS_ENDPOINT"));
			if (!sqs_endpoint.IsEmpty()) {
				client_config.endpointOverride = TCHAR_TO_UTF8(*sqs_endpoint);
			}

			// Must be longer than long polling wait time
			client_config.httpRequestTimeoutMs = 7000;
			client_config.requestTimeoutMs = 6000;

			m_sqs = MakeShared<Aws::SQS::SQSClient>(client_config);

			if (!m_handler.IsBound()) {
				UE_LOG(LogCloudConnector, Warning, TEXT("Cannot start to poll without a bound delegate"));
				return false;
			}

			return true;
		}

		uint32 Run() override {

			while (!m_interrupted) 	{
				// It may happen that this component is not yet set up once a caller says start_polling().
				// Right now I only see the way of deferring the actual start until this is done.
				// I do this by waiting for the caller to remedy this
				if (m_queue_url.empty()) 		{
					FPlatformProcess::Sleep(0.5);
					continue;
				}

				ReceiveMessageRequest rm_req;
				rm_req.SetQueueUrl(m_queue_url);
				rm_req.SetMaxNumberOfMessages(1);

				// This is not a timeout per se but long polling, which means the call will return
				// After this many seconds even if there are no messages, which is not an error.
				// See https://docs.aws.amazon.com/AWSSimpleQueueService/latest/SQSDeveloperGuide/sqs-short-and-long-polling.html#sqs-long-polling
				//
				// I am hard-chosing 4 here as values of 5 and above caused weird errors in testing.
				//
				rm_req.SetWaitTimeSeconds(4);

				// Request some additional info with each message
				rm_req.SetAttributeNames({

					// For X-Ray tracing, I need an AWSTraceHeader, which in the C++ SDK 
					// seems to be not in the enum like the others. To have it included in the response,
					// only All will do the trick.
					QueueAttributeName::All,

					QueueAttributeName::SentTimestamp,             // to calculate age
					QueueAttributeName::ApproximateReceiveCount    // to see how often that message has been received (approximation)
				});

				// This is how it's supposed to work but it doesn't. Hence the 'All' above
				rm_req.SetMessageAttributeNames({
					MessageSystemAttributeNameMapper::GetNameForMessageSystemAttributeName(MessageSystemAttributeName::AWSTraceHeader)
					});

				ReceiveMessageOutcome rm_out = m_sqs->ReceiveMessage(rm_req);
				if (!rm_out.IsSuccess()) 		{
					UE_LOG(LogCloudConnector, Warning, TEXT("Failed to retrieve message from queue '%s': '%s'"),
							UTF8_TO_TCHAR(m_queue_url.c_str()), UTF8_TO_TCHAR(rm_out.GetError().GetMessage().c_str()));
					continue;
				}

				if (rm_out.GetResult().GetMessages().empty()) 		{
					UE_LOG(LogCloudConnector, Display, TEXT("Long polling from queue '%s' returned with a timeout of 4s, no new messages"),
							UTF8_TO_TCHAR(m_queue_url.c_str()));
					continue;
				}

				UE_LOG(LogCloudConnector, Verbose, TEXT("Long polling with a timeout of 4s returned from queue '%s', %i messages"),
							UTF8_TO_TCHAR(m_queue_url.c_str()), rm_out.GetResult().GetMessages().size());

				// Process the message and call handler
				process_message(rm_out.GetResult().GetMessages()[0]);

				// Rinse, repeat until interrupted
				continue;
			}

			// I don't know what to return here
			return 0;
		}

		/// Stops the runnable object from foreign thread
		void Stop() override {

			m_interrupted.Store(true);
		}

		/// We should have no cleanup needed
		void Exit() override {}

	private:

		void process_message(const Message &n_message) const noexcept {

			UE_LOG(LogCloudConnector, Display, TEXT("processing message '%s'"), UTF8_TO_TCHAR(n_message.GetMessageId().c_str()));

			// Determine message age
			const int64_t current_epoch = Aws::Utils::DateTime::CurrentTimeMillis();
			int64_t sent_epoch = 0;
			FString trace_id;

			const Aws::Map<MessageSystemAttributeName, Aws::String> &attributes = n_message.GetAttributes();

			Aws::Map<MessageSystemAttributeName, Aws::String>::const_iterator i =
					attributes.find(MessageSystemAttributeName::SentTimestamp);
			if (i != attributes.cend()) 	{
				sent_epoch = std::stoull(i->second.c_str());
			};
			const int64_t age = current_epoch - sent_epoch;

			// Get the AWS trace header for xray
			i = attributes.find(MessageSystemAttributeName::AWSTraceHeader);
			if (i != attributes.cend()) 	{
				trace_id = UTF8_TO_TCHAR(i->second.c_str());
			};

			// Get ApproximateReceiveCount
			int received = 0;
			i = attributes.find(MessageSystemAttributeName::ApproximateReceiveCount);
			if (i != attributes.cend()) 	{
				received = std::atoi(i->second.c_str());
			};

			// The trace header comes in the form of "Root=1-235345something"
			// but all the examples I saw only use "1-235345something". With the Root=
			// xray upload doesn't work so I remove this
			trace_id = trace_id.Replace(TEXT("Root="), TEXT(""));

			// I have also seen more stuff coming after the first ID, separated by ;
			// Let's remove this as well. I wish that stuff was documented
			int32 idx;
			if (trace_id.FindChar(';', idx)) 	{
				trace_id = trace_id.Left(idx);
			}

			// Now construct a message which will be given into the handler
			FPubsubMessage msg;
			msg.m_message_id = UTF8_TO_TCHAR(n_message.GetMessageId().c_str());
			msg.m_receipt = UTF8_TO_TCHAR(n_message.GetReceiptHandle().c_str());
			msg.m_message_age = current_epoch - sent_epoch;
			msg.m_xray_header = trace_id;
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
				UE_LOG(LogCloudConnector, Display, TEXT("Msg handler returned false, not deleting message '%s'"), *msg.m_message_id);
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

		const Aws::String                m_queue_url;
		const bool                       m_handle_on_game_thread;
		const FPubsubMessageReceived     m_handler;
		TAtomic<bool>                    m_interrupted = false;
		TSharedPtr<Aws::SQS::SQSClient>  m_sqs;
};


bool AWSPubsubImpl::subscribe(const FString &n_topic, FSubscription &n_subscription, const FPubsubMessageReceived n_handler) {

	// Well, there is no subscription mechanism in SQS, so we just hook up to the Q
	
	// In here it is enough to remember the id aka Queue URL
	n_subscription.Id = n_topic;
	n_subscription.Topic = n_topic;

	{
		FScopeLock slock(&s_subscriptions_mutex);
		if (m_subscriptions.Contains(n_subscription)) {
			UE_LOG(LogCloudConnector, Error, TEXT("Already subscribed to '%s'"), *n_topic);
			return false;
		}
	}

	// So, about that tuple here. I chose to use FRunnableThread for no good reason
	// but FRunnableThread::Create() apparently does not take ownership over the impl
	// FRunnable. Which means, I have to maintain ownership myself and release it once
	// the thread is interrupted. Hence the awkwardness
	SQSSubscriptionTuple info{
		MakeUnique<SQSRunner>(n_topic, m_handle_in_game_thread, n_handler),
		nullptr
	};

	// I didn't want to rely on initialization order because in order
	// to do this here, I have to access the tuple getter and this appeared risky
	// while it is not fully initialized.
	info.Get< TUniquePtr<FRunnableThread> > ().Reset(
			FRunnableThread::Create(
					info.Get<TUniquePtr<SQSRunner>>().Get(),
					TEXT("SQSRunner")
			)
	);

	// Take ownership over both by inserting it into our subscription map
	FScopeLock slock(&s_subscriptions_mutex);
	m_subscriptions.Emplace(n_subscription, MoveTemp(info));

	return true;
}

bool AWSPubsubImpl::unsubscribe(FSubscription &&n_subscription) {

	// Unsubscribing here simply means to stop the pull

	// Locate the subscriber
	SQSSubscriptionTuple *s = nullptr;
	
	{
		FScopeLock slock(&s_subscriptions_mutex);
		s = m_subscriptions.Find(n_subscription);
		if (!s) {
			UE_LOG(LogCloudConnector, Error, TEXT("Cannot unsubscribe from '%s', internal data missing"), *n_subscription.Id);
			return false;
		}
	}
	
	// First stop the thread.
	// Killing it this way waits for the thread to join.
	s->Get< TUniquePtr<FRunnableThread> >()->Kill(true);

	// And delete all the data
	FScopeLock slock(&s_subscriptions_mutex);
	m_subscriptions.Remove(n_subscription);

	// And we're done
	UE_LOG(LogCloudConnector, Display, TEXT("Unsubscribed from '%s'"), *n_subscription.Id);
	return true;
}
