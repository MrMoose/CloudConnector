/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "ICloudPubsub.h"
#include "Utilities.h"

#include "Async/Async.h"
#include "Internationalization/Regex.h"

bool ICloudPubsub::subscribe_default(FSubscription &n_subscription, const FPubsubMessageReceived n_handler) {

	const FString default_topic = readenv("CLOUDCONNECTOR_DEFAULT_TOPIC");
	if (default_topic.IsEmpty()) {
		return false;
	}

	return subscribe(default_topic, n_subscription, n_handler);
}
