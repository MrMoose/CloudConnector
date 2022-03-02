/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

// I have not found a way to exclude those files from the build if Google Cloud is 
// not required. So I fake it blind this sway
#ifdef WITH_GOOGLECLOUD_SDK

#include "CoreMinimal.h"
#include "ICloudPubsub.h"
#include "HAL/Thread.h"

#pragma warning(push)
#pragma warning(disable:4583)
#pragma warning(disable:4582)

 // Google Cloud SDK
#include "Windows/PreWindowsApi.h"
#include "google/cloud/completion_queue.h"
#include "google/cloud/pubsub/subscription.h"
#include "google/cloud/pubsub/subscriber.h"
#include "google/cloud/pubsub/publisher.h"
#include "Windows/PostWindowsApi.h"

#pragma warning(pop)


/** IPubsub implementation using Google Cloud Pubsub
 */
class GooglePubsubImpl : public ICloudPubsub {

	public:
		GooglePubsubImpl(const class ACloudConnector *n_config);
		virtual ~GooglePubsubImpl() noexcept;

		// see ICloudPubsub docs for these
		void shutdown() noexcept override;
		bool publish(const FString &n_topic, const FString &n_message, FPubsubMessagePublished &&n_handler) override;
		bool subscribe(const FString &n_topic, FSubscription &n_subscription, FPubsubMessageReceived &&n_handler) override;
		void continue_polling(FSubscription &n_subscription) override;
		bool unsubscribe(FSubscription &&n_subscription) override;

	private:

		void receive_message(google::cloud::pubsub::Message const &n_message, 
				const FPubsubMessageReceived &n_handler, google::cloud::pubsub::AckHandler &&n_ack_handler);

		// internal information to maintain a subscription
		// including a future to shut it down.
		struct GoogleSubscriptionInfo {
			google::cloud::pubsub::Subscription           m_subscription;
			TUniquePtr<google::cloud::pubsub::Subscriber> m_subscriber;
			google::cloud::future<google::cloud::Status>  m_handle;
		};

		// A map to store them with my FSubscription info as key
		using GoogleSubscriptionMap = TMap<FSubscription, GoogleSubscriptionInfo>;

		// We store a publisher for each topic we ever talked to
		using PublisherPtr = TSharedPtr<google::cloud::pubsub::Publisher, ESPMode::ThreadSafe>;
		using GooglePublisherMap = TMap<FString, PublisherPtr>;

		const FString                  m_project_id;
		const uint32                   m_visibility_timeout;
		const bool                     m_handle_in_game_thread;
		GoogleSubscriptionMap          m_subscriptions;
		static FCriticalSection        s_subscriptions_mutex;
		GooglePublisherMap             m_publishers;
		FCriticalSection               m_publishers_mutex;

		/* The Pubsub SDK normally spawns and maintains its own background threads.
		 * However, tests have shown that I cannot seem to interact with the engine
		 * from within them. Not even things like logging. I am assuming that only 
		 * "engine" threads can do this, even though I have evidence to the contrary.
		 * Something just seems different about those threads. In any case, maintaining
		 * my own completion Q and runner solved the issue.
		 */
		google::cloud::CompletionQueue m_completion_q;
		TUniquePtr<FThread>            m_q_runner_1;    //!< background thread for the SDK polling messages
		TUniquePtr<FThread>            m_q_runner_2;    //!< and one more to be able to extend leases on messages and send messages
};

// I have not found a way to exclude those files from the build if Google Cloud is 
// not required. So I fake it blind this sway
#else // WITH_GOOGLECLOUD_SDK
#include "CoreMinimal.h"
#include "BlindPubsubImpl.h"
using GooglePubsubImpl = BlindPubsubImpl;
#endif // WITH_GOOGLECLOUD_SDK
