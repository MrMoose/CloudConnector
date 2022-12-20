/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#ifdef WITH_GOOGLECLOUD_SDK

#include "CoreMinimal.h"
#include "ICloudStorage.h"

/** ICloudStorage implementation using GoogleCloud Storage
 */
class GoogleCloudStorageImpl : public ICloudStorage {

	public:
		/** see ICloudStorage for docs
		 */
		bool exists(const FCloudStorageKey &n_key, const FCloudStorageExistsFinishedDelegate n_completion,
				ICloudTracePtr n_trace = ICloudTracePtr{}) override;

		/** see ICloudStorage for docs
		 */
		bool write(const FCloudStorageKey &n_key, const TArrayView<const uint8> n_data,
				const FCloudStorageWriteFinishedDelegate n_completion, ICloudTracePtr n_trace = ICloudTracePtr{}) override;
};

// I have not found a way to exclude those files from the build if Google Cloud is 
// not required. So I fake it blind this sway
#else // WITH_GOOGLECLOUD_SDK
#include "CoreMinimal.h"
#include "BlindStorageImpl.h"
using GoogleCloudStorageImpl = BlindStorageImpl;
#endif // WITH_GOOGLECLOUD_SDK
