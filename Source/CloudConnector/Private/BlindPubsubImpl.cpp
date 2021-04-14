/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "BlindPubsubImpl.h"
#include "ICloudConnector.h"

bool BlindPubsubImpl::subscribe(const FString &n_topic, FSubscription &n_subscription, const FPubsubMessageReceived n_handler) {

	UE_LOG(LogCloudConnector, Display, TEXT("Blind pubsub impl ignoring '%s'"), *n_subscription.Id);
	return true;
}

bool BlindPubsubImpl::unsubscribe(FSubscription &&n_subscription) {

	UE_LOG(LogCloudConnector, Display, TEXT("Blind pubsub impl unsubscribed from '%s'"), *n_subscription.Id);
	return true;
}
