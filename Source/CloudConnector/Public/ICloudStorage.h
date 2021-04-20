/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudTracing.h"
#include "Containers/ArrayView.h"

#include "ICloudStorage.generated.h"

/** A key for cloud storage of objects.
 *  At AWS this will refer to S3, for Google Cloud it's "Storage"
 */  
USTRUCT(BlueprintType, Category = "CloudConnector")
struct CLOUDCONNECTOR_API FCloudStorageKey {

	GENERATED_BODY()

	/** I am assuming that storage implementations
	 *  have some notion of a "bucket".
	 *  At least AWS and Google Cloud have it.
	 *  If other impls do not, we may reconsider this property.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BucketName;

	/** This is the object key, vulgo the filename.
	 *  It will be taken as is and not modified, so
	 *  no .jpg or whatever appended.
	 *  It must be valid within the given IStorage
	 *  implementation. For example, as a valid filename.
	 *  No sanity checks are performed outside of the impl.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ObjectKey;

	/// Implementations may take this into account if they can
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ContentType = TEXT("image/jpg");
};


/** conveys results of an exists() operation
 *  first bool is "was the operation successful"? 
 *  second bool is "is the object in storage"?
 *  FString may contain an error message
 */
DECLARE_DELEGATE_ThreeParams(FCloudStorageExistsFinishedDelegate, const bool, const bool, const FString);

/** conveys results of a write() operation
 *  first bool is "was the operation successful"?
 *  FString may contain an error message
 */
DECLARE_DELEGATE_TwoParams(FCloudStorageWriteFinishedDelegate, const bool, const FString);

/** implementation wrapper for various storage backends
 *  Operations are expected to take a little bit because it will generally
 *  involve file or network IO, depending on the impl.
 *  Therefore all operations are async
 */
class CLOUDCONNECTOR_API ICloudStorage {

	public:
		virtual ~ICloudStorage() noexcept = default;

		/** @brief tell if an object by key exists in storage
		 *  @param n_key the identifier for this object
		 *  @param n_completion will fire on the game thread when the operation is complete
		 *  @param n_trace use this to instrument the operation in a trace. Handler execution times will not be included.
		 *  @return true when the operation was successfully started, in which case the delegate will always fire
		 */
		virtual bool exists(const FCloudStorageKey &n_key, const FCloudStorageExistsFinishedDelegate n_completion, 
				CloudTracePtr n_trace = CloudTracePtr{}) = 0;

		/** @brief write the contents of the buffer to storage
		 *  potentially existing object of same key may or may not be overwritten.
		 *  Decision is at the impl.
		 *  @param n_key the identifier for the new object
		 *  @param n_data must not be null. Underlying buffer must remain valid until delegate fires.
		 *			This is the caller's responsibility.
		 *  @param n_completion will fire on the game thread when the operation is complete
		 *                 note that you don't have to bind this if the result is not important.
		 *  @param n_trace use this to instrument the operation in a trace. Handler execution times will not be included.            
		 *  @return true when the operation was successfully started, in which case the delegate will always fire
		 */
		virtual bool write(const FCloudStorageKey &n_key, const TArrayView<const uint8> n_data,
				const FCloudStorageWriteFinishedDelegate n_completion, CloudTracePtr n_trace = CloudTracePtr{}) = 0;

};

