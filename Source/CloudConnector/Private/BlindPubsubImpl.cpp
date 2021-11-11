/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "BlindPubsubImpl.h"
#include "ICloudConnector.h"

void BlindPubsubImpl::shutdown() noexcept {

	UE_LOG(LogCloudConnector, Display, TEXT("Blind pubsub impl shutting down"));
	return;
}

bool BlindPubsubImpl::publish(const FString &n_topic, const FString &n_message, FPubsubMessagePublished &&n_handler) {
	
	UE_LOG(LogCloudConnector, Display, TEXT("Blind pubsub impl won't post message to '%s'"), *n_topic);
	return false;
}

bool BlindPubsubImpl::subscribe(const FString &n_topic, FSubscription &n_subscription, FPubsubMessageReceived &&n_handler) {

	UE_LOG(LogCloudConnector, Display, TEXT("Blind pubsub impl ignoring '%s'"), *n_subscription.Id);
	return true;
}

bool BlindPubsubImpl::unsubscribe(FSubscription &&n_subscription) {

	UE_LOG(LogCloudConnector, Display, TEXT("Blind pubsub impl unsubscribed from '%s'"), *n_subscription.Id);
	return true;
}
