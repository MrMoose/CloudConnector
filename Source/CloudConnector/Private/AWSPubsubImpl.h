/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudPubsub.h"

namespace Aws::SQS {
	class SQSClient;
}


/** IPubsub implementation using AWS SQS
 */
class AWSPubsubImpl : public ICloudPubsub {

	public:
		AWSPubsubImpl(const bool n_handle_in_game_thread);
		virtual ~AWSPubsubImpl() noexcept;

		bool subscribe(const FString &n_topic, FSubscription &n_subscription, const FPubsubMessageReceived n_handler) override;

		bool unsubscribe(FSubscription &&n_subscription) override;

	private:
		// internal information to maintain a subscription
		// including a future to shut it down.
		using SQSSubscriptionTuple = TTuple<
				TUniquePtr<class SQSRunner>,
				TUniquePtr<FRunnableThread>
		>;

		// A map to store them with my FSubscription info as key
		//using SQSSubscriptionMap = TMap<FSubscription, TSharedPtr<class SQSRunner> >;
		using SQSSubscriptionMap = TMap<FSubscription, SQSSubscriptionTuple>;

		const bool          m_handle_in_game_thread;
		SQSSubscriptionMap  m_subscriptions;
};
