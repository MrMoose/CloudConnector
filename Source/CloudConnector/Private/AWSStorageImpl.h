/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudStorage.h"

/** ICloudStorage implementation using AWS S3
 */
class AWSStorageImpl : public ICloudStorage {

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
