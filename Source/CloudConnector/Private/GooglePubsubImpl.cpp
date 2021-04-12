/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "GooglePubsubImpl.h"
#include "ICloudConnector.h"
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

//#include <algorithm>

namespace gc = google::cloud;
namespace pubsub = google::cloud::pubsub;


GooglePubsubImpl::GooglePubsubImpl(const FString &n_project_id)
		: m_project_id{ n_project_id } {

	/*
	std::generate_n(std::back_inserter(m_runners), 4,
		[this] {
			return std::thread(
				[this]() mutable {
					this->m_completion_q.Run();
				}
			);
		}
	);
	*/

	m_runner.Reset(new FThread(TEXT("Google Pubsub Runner"),
		[this] {
			this->m_completion_q.Run();
		}
	));
}

GooglePubsubImpl::~GooglePubsubImpl() {

	m_completion_q.Shutdown();

	/*
	for (std::thread &t : m_runners) {
		t.join();
	}
	*/

	m_runner->Join();
	m_runner.Reset();
}

bool GooglePubsubImpl::subscribe(const FString &n_topic, FSubscription &n_subscription, const FPubsubMessageReceived n_handler) {


	// First we need to create a subscription for ourselves. 
	// This will enable us to receive messages
	// I will just make this up as I go along trying to understand pubsub 
	// and how it could best match to SQS

	// So instead of relying on the user to create a subscription for us, I do it

	// First we need a name for our subscription. Let's try this...

	const FString instance_id = get_google_cloud_instance_id();

	n_subscription.Topic = n_topic;
	n_subscription.Id = FString::Printf(TEXT("CloudConnector-%s-%s"), *instance_id, *n_subscription.Topic);

	// So, let's see if we can create a subscription for us
	google::cloud::pubsub::SubscriptionAdminClient subscription_admin_client(
		google::cloud::pubsub::MakeSubscriptionAdminConnection());

	const pubsub::Topic topic(TCHAR_TO_UTF8(*m_project_id), TCHAR_TO_UTF8(*n_subscription.Topic));
	const pubsub::Subscription sub(TCHAR_TO_UTF8(*m_project_id), TCHAR_TO_UTF8(*n_subscription.Id));

	pubsub::SubscriptionBuilder options;
	options.set_ack_deadline(std::chrono::seconds(ICloudPubsub::VisibilityTimeout));
	options.set_retain_acked_messages(false);

	gc::StatusOr<google::pubsub::v1::Subscription> subscription = subscription_admin_client.CreateSubscription(topic, sub, options);
	
	if (subscription.status().code() == google::cloud::StatusCode::kAlreadyExists) {
		UE_LOG(LogCloudConnector, Warning, TEXT("Subscription '%s' already exists"), *n_subscription.Id);
	}

	if (!subscription) {
		UE_LOG(LogCloudConnector, Error, TEXT("Could not create Subscription '%s' to topic '%s': %s"), 
				*n_subscription.Id, *n_subscription.Topic, UTF8_TO_TCHAR(subscription.status().message().c_str()));
		return false;
	}

	// Now we should have a subscription for us. Next step is to hook up to it.
	pubsub::SubscriberOptions subscriber_options;
	subscriber_options.set_max_concurrency(2);

	pubsub::ConnectionOptions connection_options;
	connection_options.set_background_thread_pool_size(1);  // This is default but I can't have concurrency in there.
	
	pubsub::Subscriber subscriber(pubsub::MakeSubscriberConnection(
		sub,
		subscriber_options,
		connection_options));



	auto sample = [](pubsub::Subscriber subscriber) {
		return subscriber.Subscribe(
			[&](pubsub::Message const &m, pubsub::AckHandler h) {
				std::cout << "Received message " << m << "\n";
				std::move(h).ack();
				
			});
	};

	return {};
}
