/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "AWSQueueImpl.h"
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
#include <aws/sqs/model/ReceiveMessageRequest.h>
#include <aws/sqs/model/ReceiveMessageResult.h>
#include <aws/sqs/model/DeleteMessageRequest.h>
#include <aws/sqs/model/ChangeMessageVisibilityRequest.h>
#include "Windows/PostWindowsApi.h"

#include <atomic>

using namespace Aws::SQS::Model;

AWSQueueImpl::AWSQueueImpl(const bool n_handle_in_game_thread)
		: m_handle_in_game_thread{ n_handle_in_game_thread } {

}

AWSQueueImpl::~AWSQueueImpl() noexcept {

	shutdown();
}

void AWSQueueImpl::shutdown() noexcept {

	m_shut_down.store(true);
	shutdown_runners(nullptr);
}

void AWSQueueImpl::shutdown_runners(FViewport *n_viewport) noexcept {

	try {
		// user may have not unsubscribed
		TArray<FQueueSubscription> remaining_subs;
		remaining_subs.Reserve(m_subscriptions.Num());
		{
			FScopeLock slock(&m_subscriptions_mutex);
			for (const TPair<FQueueSubscription, TUniquePtr<SQSRunner> > &i : m_subscriptions) {
				remaining_subs.Add(i.Key);
			}
		}

		for (FQueueSubscription &s : remaining_subs) {
			stop_listen(MoveTemp(s));
		}

		// de-register from the Alt+F4 delegate
		if (m_emergency_shutdown_handle.IsValid() && GEngine && GEngine->GameViewport) {

			GEngine->GameViewport->OnCloseRequested().Remove(m_emergency_shutdown_handle);
		}

	} catch (const std::exception &sex) {
		UE_LOG(LogCloudConnector, Error, TEXT("Unexpected exception tearing down SQS: %s"), UTF8_TO_TCHAR(sex.what()));
	}
};

class SQSRunner {

	public:
		SQSRunner(const FString &n_queue_url, const bool n_handle_on_game_thread, const FQueueMessageReceived n_handler)
				: m_queue_url{ TCHAR_TO_UTF8(*n_queue_url) }
				, m_handle_on_game_thread{ n_handle_on_game_thread }
				, m_handler{ n_handler } {

			// Sanity please
			checkf(m_handler.IsBound(), TEXT("Handler for SQS must be bound"));

			// And fire up a thread to run while we're gone
			m_runner = MakeUnique<FThread>(TEXT("AWS SQS Runner"), [this] { this->run(); });
		}

		void run() noexcept {

			m_sqs = aws_client_factory<Aws::SQS::SQSClient>::create();

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

			// Extract trace id
			const FString trace_id = [&] {

				FString ret;

				// Get the AWS trace header for xray
				i = attributes.find(MessageSystemAttributeName::AWSTraceHeader);
				if (i != attributes.cend()) {
					ret = UTF8_TO_TCHAR(i->second.c_str());
				} else {
					return ret;
				};

				// The trace header comes in the form of "Root=1-235345something"
				// but all the examples I saw only use "1-235345something". With the Root=
				// xray upload doesn't work so I remove this
				ret = ret.Replace(TEXT("Root="), TEXT(""));

				// I have also seen more stuff coming after the first ID, separated by ;
				// Let's remove this as well. I wish that stuff was documented
				int32 idx;
				if (ret.FindChar(';', idx)) {
					ret = ret.Left(idx);
				}
				
				return ret;
			} ();
			
			// Now construct a message which will be given into the handler
			FQueueMessage msg;
			msg.m_aws_sqs_message_id = UTF8_TO_TCHAR(n_message.GetMessageId().c_str());
			msg.m_aws_sqs_receipt = UTF8_TO_TCHAR(n_message.GetReceiptHandle().c_str());
			msg.m_message_age = current_epoch - sent_epoch;
			msg.m_receive_count = received;
			msg.m_body = UTF8_TO_TCHAR(n_message.GetBody().c_str());
			
			// If we have a trace id, we construct a fresh trace right away
			if (!trace_id.IsEmpty()) {
				msg.m_trace = ICloudConnector::Get().tracing().start_trace(trace_id);
			}

			// This promise will be fulfilled by the delegate implementation
			const QueueReturnPromisePtr rp = MakeShared<QueueReturnPromise, ESPMode::ThreadSafe>();
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

		const Aws::String                    m_queue_url;
		const bool                           m_handle_on_game_thread;
		const FQueueMessageReceived          m_handler;
		std::atomic<bool>                    m_interrupted = false;
		ReceiveMessageOutcomeCallable        m_future_out;
		Aws::UniquePtr<Aws::SQS::SQSClient>  m_sqs;
		TUniquePtr<FThread>                  m_runner;
};


bool AWSQueueImpl::listen(const FString &n_topic, FQueueSubscription &n_subscription, const FQueueMessageReceived n_handler) {

	if (m_shut_down) {
		UE_LOG(LogCloudConnector, Display, TEXT("AWS Queue Object is being shut down. Ignore subscribe cmd"));
		return false;
	}

	// Well, there is no subscription mechanism in SQS, so we just hook up to the Q
	
	// In here it is enough to remember the id aka Queue URL
	n_subscription.Id = n_topic;
	n_subscription.Queue = n_topic;

	{
		FScopeLock slock(&m_subscriptions_mutex);
		if (m_subscriptions.Contains(n_subscription)) {
			UE_LOG(LogCloudConnector, Error, TEXT("Already subscribed to '%s'"), *n_topic);
			return false;
		}
	}

	TUniquePtr<SQSRunner> runner = MakeUnique<SQSRunner>(n_topic, m_handle_in_game_thread, n_handler);

	// hooko up to Unreal's Alt+F4 catch-all delegate to shut down SQS gracefully in this case
	if (!m_emergency_shutdown_handle.IsValid() && GEngine && GEngine->GameViewport) {

		m_emergency_shutdown_handle = GEngine->GameViewport->OnCloseRequested().AddRaw(this, &AWSQueueImpl::shutdown_runners);
	}

	// Take ownership over both by inserting it into our subscription map
	FScopeLock slock(&m_subscriptions_mutex);
	m_subscriptions.Emplace(n_subscription, MoveTemp(runner));

	return true;
}

bool AWSQueueImpl::stop_listen(FQueueSubscription &&n_subscription) {
	
	if (m_shut_down) {
		UE_LOG(LogCloudConnector, Display, TEXT("AWS Queue Object is being shut down. Ignore unsubscribe cmd"));
		return false;
	}

	// Unsubscribing here simply means to stop the pull
	
	// Locate the subscriber
	TUniquePtr<SQSRunner> *r = nullptr;
	
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
