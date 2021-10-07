/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudPubsub.h"

#include "Templates/UniquePtr.h"

#include <atomic>

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

		void shutdown() noexcept override;

		// React on Alt+F4 to shutdown all pending runners		
		void shutdown_runners(class FViewport *) noexcept;

	private:
		
		// internal information to maintain a subscription
		// A map to store them with my FSubscription info as key
		using SQSSubscriptionMap = TMap<FSubscription, TUniquePtr<class SQSRunner> >;

		const bool              m_handle_in_game_thread;
		SQSSubscriptionMap      m_subscriptions;
		FCriticalSection        m_subscriptions_mutex;
		std::atomic<bool>       m_shut_down = false;         // Will be set true once we are shut down and don't accept any calls

		FDelegateHandle         m_emergency_shutdown_handle;
};
