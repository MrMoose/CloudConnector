/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudPubsub.h"

/** default IStorage implementation using files on local disk
 */
class AWSPubsubImpl : public ICloudPubsub {

	public:
		bool subscribe(const FString &n_topic, FSubscription &n_subscription, const FPubsubMessageReceived n_handler) override;
};
