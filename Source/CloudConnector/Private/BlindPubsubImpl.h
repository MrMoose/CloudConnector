/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudPubsub.h"

/** IPubsub implementation doing nothing
 */
class BlindPubsubImpl : public ICloudPubsub {

	public:
		bool subscribe(const FString &n_topic, FSubscription &n_subscription, const FPubsubMessageReceived n_handler) override;
		bool unsubscribe(FSubscription &&n_subscription) override;
};
