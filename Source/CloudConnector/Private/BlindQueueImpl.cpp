/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "BlindQueueImpl.h"
#include "ICloudConnector.h"


void BlindQueueImpl::shutdown() noexcept {

	UE_LOG(LogCloudConnector, Display, TEXT("Blind queue impl shutdown request ignored"));
}

bool BlindQueueImpl::listen(const FString &n_queue, FQueueSubscription &n_subscription, const FQueueMessageReceived n_handler) {

	UE_LOG(LogCloudConnector, Display, TEXT("Blind queue impl ignoring '%s'"), *n_subscription.Id);
	return true;
}

bool BlindQueueImpl::stop_listen(FQueueSubscription &&n_subscription) {

	UE_LOG(LogCloudConnector, Display, TEXT("Blind queue impl unsubscribed from '%s'"), *n_subscription.Id);
	return true;
}
