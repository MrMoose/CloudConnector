/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudPubsub.h"
#include "Templates/Tuple.h"
#include "HAL/Thread.h"

#pragma warning(push)
#pragma warning(disable:4583)
#pragma warning(disable:4582)

 // Google Cloud SDK
#include "Windows/PreWindowsApi.h"
#include "google/cloud/completion_queue.h"
#include "google/cloud/pubsub/subscription.h"
#include "google/cloud/pubsub/subscriber.h"
#include "Windows/PostWindowsApi.h"

#pragma warning(pop)


/** IPubsub implementation using Google Cloud Pubsub
 */
class GooglePubsubImpl : public ICloudPubsub {

	public:
		GooglePubsubImpl(const FString &n_project_id, const bool n_handle_in_game_thread);
		virtual ~GooglePubsubImpl() noexcept;

		// see ICloudPubsub docs for these
		bool subscribe(const FString &n_topic, FSubscription &n_subscription, const FPubsubMessageReceived n_handler) override;
		bool unsubscribe(FSubscription &&n_subscription) override;

	private:

		void receive_message(google::cloud::pubsub::Message const &n_message, 
				const FPubsubMessageReceived &n_handler, google::cloud::pubsub::AckHandler &&n_ack_handler);

		// internal information to maintain a subscription
		// including a future to shut it down.
		using GoogleSubscriptionTuple = TTuple<
				google::cloud::pubsub::Subscription,
				TUniquePtr<google::cloud::pubsub::Subscriber>,
				google::cloud::future<google::cloud::Status>
		>;

		// A map to store them with my FSubscription info as key
		using GoogleSubscriptionMap = TMap<FSubscription, GoogleSubscriptionTuple>;

		const FString                  m_project_id;
		const bool                     m_handle_in_game_thread;
		GoogleSubscriptionMap          m_subscriptions;

		/* The Pubsub SDK normally spawns and maintains its own background threads.
		 * However, tests have shown that I cannot seem to interact with the engine
		 * from within them. Not even things like logging. I am assuming that only 
		 * "engine" threads can do this, even though I have evidence to the contrary.
		 * Something just seems different about those threads. In any case, maintaining
		 * my own completion Q and runner solved the issue.
		 */
		google::cloud::CompletionQueue m_completion_q;
		TUniquePtr<FThread>            m_runner;   //!< background thread for the SDK
};
