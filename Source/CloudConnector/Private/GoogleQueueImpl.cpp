/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */

#ifdef WITH_GOOGLECLOUD_SDK

#include "GoogleQueueImpl.h"
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

#include <future>

namespace gc = google::cloud;
namespace pubsub = google::cloud::pubsub;

GoogleQueueImpl::GoogleQueueImpl(const FString &n_project_id, const bool n_handle_in_game_thread) {

}

GoogleQueueImpl::~GoogleQueueImpl() noexcept {

}

void GoogleQueueImpl::shutdown() noexcept {

}

bool GoogleQueueImpl::listen(const FString &n_queue, FQueueSubscription &n_subscription, const FQueueMessageReceived n_handler) {

	UE_LOG(LogCloudConnector, Display, TEXT("Google queue (Taks) not implemented ignoring '%s'"), *n_subscription.Id);
	return true;
}

bool GoogleQueueImpl::stop_listen(FQueueSubscription &&n_subscription) {

	UE_LOG(LogCloudConnector, Display, TEXT("Google queue (Taks) not implemented"));
	return true;
};

#endif // WITH_GOOGLECLOUD_SDK
