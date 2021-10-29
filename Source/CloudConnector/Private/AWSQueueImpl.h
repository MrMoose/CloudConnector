/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudQueue.h"

#include "Templates/UniquePtr.h"

#include <atomic>

namespace Aws::SQS {
	class SQSClient;
}


/** ICloudQueue implementation using AWS SQS
 */
class AWSQueueImpl : public ICloudQueue {

	public:
		AWSQueueImpl(const class ACloudConnector *n_config);
		virtual ~AWSQueueImpl() noexcept;

		bool listen(const FString &n_queue, FQueueSubscription &n_subscription, const FQueueMessageReceived n_handler) override;

		bool stop_listen(FQueueSubscription &&n_subscription) override;

		void shutdown() noexcept override;

		// React on Alt+F4 to shutdown all pending runners		
		void shutdown_runners(class FViewport *) noexcept;

	private:
		
		// internal information to maintain a subscription
		// A map to store them with my FQueueSubscription info as key
		using SQSSubscriptionMap = TMap<FQueueSubscription, TUniquePtr<class SQSRunner> >;
		
		const bool              m_handle_in_game_thread;
		SQSSubscriptionMap      m_subscriptions;
		FCriticalSection        m_subscriptions_mutex;
		std::atomic<bool>       m_shut_down = false;         // Will be set true once we are shut down and don't accept any calls

		FDelegateHandle         m_emergency_shutdown_handle;
};
