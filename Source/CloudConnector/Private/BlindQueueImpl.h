/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudQueue.h"

/** ICloudQueue implementation doing nothing
 */
class BlindQueueImpl : public ICloudQueue {

	public:
		void shutdown() noexcept override;
		bool listen(const FString &n_queue, FQueueSubscription &n_subscription, const FQueueMessageReceived n_handler) override;
		bool stop_listen(FQueueSubscription &&n_subscription) override;
};
