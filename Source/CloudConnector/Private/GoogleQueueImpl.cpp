/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */

#ifdef WITH_GOOGLECLOUD_SDK

#include "GoogleQueueImpl.h"
#include "ICloudConnector.h"
#include "CloudConnector.h"
#include "Utilities.h"

#include "Async/Async.h"
#include "Internationalization/Regex.h"

 // GoogleCloud SDK uses a few Cpp features that cause problems in Unreal Context.
 // See here: https://github.com/akrzemi1/Optional/issues/57 for an explanation

#pragma warning(push)
#pragma warning(disable:4583)
#pragma warning(disable:4582)

 // Google Cloud SDK
#include "Windows/PreWindowsApi.h"
#include "google/cloud/pubsub/subscriber.h"
#include "google/cloud/pubsub/subscription_admin_client.h"
#include "Windows/PostWindowsApi.h"

#pragma warning(pop)

#include <future>

namespace gc = google::cloud;
namespace pubsub = google::cloud::pubsub;

FCriticalSection GoogleQueueImpl::s_subscriptions_mutex;

GoogleQueueImpl::GoogleQueueImpl(const ACloudConnector *n_config)
		: m_project_id{ google_project_id(n_config->GoogleProjectId) }
		, m_visibility_timeout{ visibility_timeout(n_config->VisibilityTimeout) }
		, m_handle_in_game_thread{ n_config->HandleOnGameThread } {

	m_runner.Reset(new FThread(TEXT("Google Pubsub Runner"),
		[this] {
			this->m_completion_q.Run();
		}
	));
}

GoogleQueueImpl::~GoogleQueueImpl() noexcept {

	shutdown();
}

void GoogleQueueImpl::shutdown() noexcept {

	try {
		// user may have not unsubscribed
		TArray<FQueueSubscription> remaining_subs;

		{
			FScopeLock slock(&s_subscriptions_mutex);
			remaining_subs.Reserve(m_subscriptions.Num());
			for (const TPair<FQueueSubscription, GoogleSubscriptionTuple> &i : m_subscriptions) {
				remaining_subs.Add(i.Key);
			}
		}

		for (FQueueSubscription &s : remaining_subs) {
			stop_listen(MoveTemp(s));
		}

		if (m_runner.IsValid()) {
			m_completion_q.Shutdown();

			m_runner->Join();
			m_runner.Reset();
		}

	} catch (const std::exception &sex) {
		UE_LOG(LogCloudConnector, Error, TEXT("Unexpected exception tearing down pubsub: %s"), UTF8_TO_TCHAR(sex.what()));
	}
}

bool GoogleQueueImpl::listen(const FString &n_queue, FQueueSubscription &n_subscription, const FQueueMessageReceived n_handler) {

	// First we need to create a subscription for ourselves. 
	// This will enable us to receive messages.
	// I just make this up as I go along trying to understand pubsub 
	// and how it could best match to SQS behavior.

	// So instead of relying on the user to create a subscription for us, I do it

	// First we need a name for our subscription. Let's try this...
	const std::string instance_id = get_google_cloud_instance_id();
	n_subscription.Queue = n_queue;
	n_subscription.Id = FString::Printf(TEXT("%s-%s-subscription"), UTF8_TO_TCHAR(instance_id.c_str()), *n_subscription.Queue);

	{
		FScopeLock slock(&s_subscriptions_mutex);
		if (m_subscriptions.Contains(n_subscription)) {
			UE_LOG(LogCloudConnector, Error, TEXT("Already subscribed to '%s'"), *n_queue);
			return false;
		}
	}

	// So, let's see if we can create a subscription for us
	pubsub::SubscriptionAdminClient subscription_admin_client(pubsub::MakeSubscriptionAdminConnection());

	const pubsub::Topic topic(TCHAR_TO_UTF8(*m_project_id), TCHAR_TO_UTF8(*n_subscription.Queue));
	pubsub::Subscription sub(TCHAR_TO_UTF8(*m_project_id), TCHAR_TO_UTF8(*n_subscription.Id));

	// Subscription parameters
	pubsub::SubscriptionBuilder options;
	options.set_ack_deadline(std::chrono::seconds(m_visibility_timeout));
	options.set_retain_acked_messages(false);

	// Create it. This can take a while.
	gc::StatusOr<google::pubsub::v1::Subscription> subscription = subscription_admin_client.CreateSubscription(topic, sub, options);

	if (subscription.status().code() == google::cloud::StatusCode::kAlreadyExists) {
		UE_LOG(LogCloudConnector, Warning, TEXT("Subscription '%s' already exists"), *n_subscription.Id);
	} else {
		if (!subscription) {
			UE_LOG(LogCloudConnector, Error, TEXT("Could not create Subscription '%s' to queue '%s': %s"),
					*n_subscription.Id, *n_subscription.Queue, UTF8_TO_TCHAR(subscription.status().message().c_str()));
			return false;
		}
	}

	UE_LOG(LogCloudConnector, Display, TEXT("Successfully created subscription '%s'"), *n_subscription.Id);

	// Now we should have a subscription for us. Next step is to hook up to it.
	gc::Options sub_options;
	sub_options.set<pubsub::MaxConcurrencyOption>(1);
	sub_options.set<gc::GrpcCompletionQueueOption>(m_completion_q);

	// create a "Subscriber", which is basically a runner for one subscription 
	// with no thread pool as we run the thread ourselves. I don't know yet if it is
	// a problem to potentially have multiple subscribers working on this one completion Q
	TUniquePtr<pubsub::Subscriber> subscriber = MakeUnique<pubsub::Subscriber>(pubsub::MakeSubscriberConnection(
		sub,
		sub_options));

	// remember our subscription in a map
	GoogleSubscriptionTuple *new_entry = nullptr;
	{
		FScopeLock slock(&s_subscriptions_mutex);
		m_subscriptions.Emplace(n_subscription, GoogleSubscriptionTuple{
					MoveTemp(sub),
					MoveTemp(subscriber),
					gc::future<gc::Status>{}
			}
		);
		new_entry = m_subscriptions.Find(n_subscription);
	}

	checkf(new_entry, TEXT("Internal data failure when inserting new subscription"));

	// Yes, we have race here in case multiple subscribers do this at 
	// once and cause rebalancing of the map but I take the liberty of ignoring this for now.

	pubsub::Subscriber &s = *new_entry->Get< TUniquePtr<pubsub::Subscriber> >().Get();

	// Hook up our handler function
	new_entry->Get< gc::future<gc::Status> >() = s.Subscribe(
		[this, n_handler](pubsub::Message const &n_message, pubsub::AckHandler n_ack_handler) {

			// call message processing function which will do the rest
			this->receive_message(n_message, n_handler, std::move(n_ack_handler));
		});

	UE_LOG(LogCloudConnector, Display, TEXT("Now subscribed to '%s', receiving messages"), *n_subscription.Id);

	return true;
}

void GoogleQueueImpl::receive_message(pubsub::Message const &n_message, const FQueueMessageReceived &n_handler, pubsub::AckHandler &&n_ack_handler) {

	// We have received a message. We are in the worker thread here.
	FQueueMessage message;
	message.m_google_pubsub_message_id = UTF8_TO_TCHAR(n_message.message_id().c_str());
	message.m_body = UTF8_TO_TCHAR(n_message.data().c_str());

	const std::chrono::system_clock::time_point now{ std::chrono::system_clock::now() };
	message.m_message_age = std::chrono::duration_cast<std::chrono::milliseconds>(now - n_message.publish_time()).count();

	UE_LOG(LogCloudConnector, Display, TEXT("Received message (%s)"), *message.m_google_pubsub_message_id);

	// Now I create the return Promise. It will be fulfilled by the delegate implementation
	const QueueReturnPromisePtr rp = MakeShared<QueueReturnPromise, ESPMode::ThreadSafe>();
	TFuture<bool> return_future = rp->GetFuture();

	// Call the delegate on the game thread or right here, depending on what the user desires
	if (m_handle_in_game_thread) {
		Async(EAsyncExecution::TaskGraphMainThread, [message, n_handler, rp] {
			n_handler.ExecuteIfBound(message, rp);
		});
	} else {
		n_handler.ExecuteIfBound(message, rp);
	}

	// Wait for the delegate impl to satisfy the promise.
	// This might take forever if the implementation is not careful.
	// Maybe agree on a timeout?
	return_future.Wait();
	if (return_future.Get()) {
		// Test have shown that acknowledging is message has similar semantics as
		// deleting one in SQS. I couldn't find any means to delete a message otherwise
		// so that seems to be all I can do for now.
		std::move(n_ack_handler).ack();
		UE_LOG(LogCloudConnector, Verbose, TEXT("Acknowledged message '%s', handler returned true"), *message.m_google_pubsub_message_id);
	} else {
		std::move(n_ack_handler).nack();
		UE_LOG(LogCloudConnector, Display, TEXT("Not acknowledging message '%s', handler returned false"), *message.m_google_pubsub_message_id);
	}
}

bool GoogleQueueImpl::stop_listen(FQueueSubscription &&n_subscription) {

	// Locate the subscriber
	GoogleSubscriptionTuple *s = nullptr;
	{
		FScopeLock slock(&s_subscriptions_mutex);
		s = m_subscriptions.Find(n_subscription);
		if (!s) {
			UE_LOG(LogCloudConnector, Error, TEXT("Cannot unsubscribe from '%s', internal data missing"), *n_subscription.Id);
			return false;
		}
	}

	// Stop the subscriber. cancel() on the future is said to halt message retrieval
	s->Get< gc::future<gc::Status> >().cancel();

	// Do I have to wait? In Tests this returned right away but still I feel it's safer.
	const std::future_status status = s->Get< gc::future<gc::Status> >().wait_for(std::chrono::seconds(4));

	// Delete internal structs and subscriber
	{
		FScopeLock slock(&s_subscriptions_mutex);
		m_subscriptions.Remove(n_subscription);
	}

	// Now delete the subscription we created for us
	pubsub::SubscriptionAdminClient subscription_admin_client(pubsub::MakeSubscriptionAdminConnection());

	const pubsub::Subscription sub(TCHAR_TO_UTF8(*m_project_id), TCHAR_TO_UTF8(*n_subscription.Id));
	const gc::Status stat = subscription_admin_client.DeleteSubscription(sub);

	if (!stat.ok()) {
		UE_LOG(LogCloudConnector, Warning, TEXT("Failed to delete subscription '%s': %s"),
				*n_subscription.Id, UTF8_TO_TCHAR(stat.message().c_str()));
	} else {
		UE_LOG(LogCloudConnector, Display, TEXT("Deleted subscription '%s'"), *n_subscription.Id);
	}

	return true;
};

#endif // WITH_GOOGLECLOUD_SDK
