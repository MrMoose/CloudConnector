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

		void shutdown() noexcept;
		bool publish(const FString &n_topic, const FString &n_message, FPubsubMessagePublished &&n_handler);
		bool subscribe(const FString &n_topic, FSubscription &n_subscription, FPubsubMessageReceived &&n_handler) override;
		void continue_polling(FSubscription &n_subscription) override;
		bool unsubscribe(FSubscription &&n_subscription) override;
};
