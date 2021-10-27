/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

// I have not found a way to exclude those files from the build if Google Cloud is 
// not required. So I fake it blind this sway
#ifdef WITH_GOOGLECLOUD_SDK

#include "CoreMinimal.h"
#include "ICloudQueue.h"
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


/** ICloudQueue implementation using Google Cloud Tasks to implement
	queuing. Due to lack of SDK support this is now a blind impl.
	Do not use it
 */
class GoogleQueueImpl : public ICloudQueue {

	public:
		GoogleQueueImpl(const FString &n_project_id, const bool n_handle_in_game_thread);
		virtual ~GoogleQueueImpl() noexcept;

		void shutdown() noexcept override;

		// see ICloudQueue docs for these
		bool listen(const FString &n_queue, FQueueSubscription &n_subscription, const FQueueMessageReceived n_handler) override;
		bool stop_listen(FQueueSubscription &&n_subscription) override;
		
};

// I have not found a way to exclude those files from the build if Google Cloud is 
// not required. So I fake it blind this sway
#else // WITH_GOOGLECLOUD_SDK
#include "CoreMinimal.h"
#include "BlindQueueImpl.h"
using GooglePubsubImpl = BlindPubsubImpl;
#endif // WITH_GOOGLECLOUD_SDK
