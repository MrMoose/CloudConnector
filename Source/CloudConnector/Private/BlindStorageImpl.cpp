/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "BlindStorageImpl.h"

bool BlindStorageImpl::exists(const FCloudStorageKey &n_key, const FCloudStorageExistsFinishedDelegate n_completion) {

	n_completion.ExecuteIfBound(true, false, *FString::Printf(TEXT("'%s' might as well not exist"), *n_key.ObjectKey));	
	return true;
}

bool BlindStorageImpl::write(const FCloudStorageKey &n_key, const TArrayView<const uint8> n_data,
		const FCloudStorageWriteFinishedDelegate n_completion) {

	n_completion.ExecuteIfBound(true, *FString::Printf(TEXT("'%s' would have been going somewhere"), *n_key.ObjectKey));
	return true;
}
