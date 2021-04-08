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
		FSubscription subscribe(const FString &n_topic, const FPubsubMessageReceived n_handler) override;
};
