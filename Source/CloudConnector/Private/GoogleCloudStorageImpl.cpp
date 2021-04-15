/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "GoogleCloudStorageImpl.h"
#include "ICloudConnector.h"

#include "Async/Async.h"
#include "Internationalization/Regex.h"

// GoogleCloud SDK uses a few Cpp features that cause problems in Unreal Context.
// See here: https://github.com/akrzemi1/Optional/issues/57 for an explanation

#pragma warning(push)
#pragma warning(disable:4583)
#pragma warning(disable:4582)

// Google Cloud SDK. Surprisingly few headers
#include "Windows/PreWindowsApi.h"
#include "google/cloud/storage/client.h"
#include "Windows/PostWindowsApi.h"
#pragma warning(pop)

// std
#include <iostream>
#include <memory>
#include <fstream>

namespace gc  = google::cloud;
namespace gcs = google::cloud::storage;

namespace {

inline bool valid_google_bucket_name(const FString &n_bucket_name) {

	// haven't found a regex yet. Make one!
	// see https://cloud.google.com/storage/docs/naming-buckets

	return true;
}

inline bool valid_google_object_key(const FString &n_object_key) {

	// haven't found a regex yet. Make one!
	// see https://cloud.google.com/storage/docs/naming-objects

	return true;
}

} // anon ns


bool GoogleCloudStorageImpl::exists(const FCloudStorageKey &n_key, const FCloudStorageExistsFinishedDelegate n_completion) {

	// Let's do some sanity checks on the bucket and key names
	if (!valid_google_bucket_name(n_key.BucketName)) {
		return false;
	}

	if (!valid_google_object_key(n_key.ObjectKey)) {
		return false;
	}

	// go async here and do the actual network IO in the thread pool
	Async(EAsyncExecution::ThreadPool, [n_key, n_completion]{

		const std::string bucket_name{ TCHAR_TO_ANSI(*n_key.BucketName) };
		const std::string object_key{ TCHAR_TO_ANSI(*n_key.ObjectKey) };

		// This appears to be the recommended way of creating the GC client.
		// It's basically an optional holding either the client or an error message
		gc::StatusOr<gcs::Client> client = gcs::Client::CreateDefaultClient();
		if (!client) {
			const FString msg = FString::Printf(TEXT("Failed to create client for Google Storage bucket '%s': %s"),
				*n_key.BucketName, UTF8_TO_TCHAR(client.status().message().c_str()));
			UE_LOG(LogCloudConnector, Warning, TEXT("%s"), *msg);

			Async(EAsyncExecution::TaskGraphMainThread, [msg, n_completion] {
				n_completion.ExecuteIfBound(false, false, msg);
			});
			return;
		}

		gc::StatusOr<gcs::ObjectMetadata> object_metadata = client->GetObjectMetadata(bucket_name, object_key);

		if (!object_metadata) {
			// I'm assuming this means the object ain't there. Which is an expected response
			if (object_metadata.status().code() == gc::StatusCode::kNotFound) {
				const FString msg = FString::Printf(TEXT("Object '%s' was not found in bucket '%s'"),
						*n_key.ObjectKey, *n_key.BucketName);
				UE_LOG(LogCloudConnector, Display, TEXT("%s"), *msg);

				Async(EAsyncExecution::TaskGraphMainThread, [msg, n_completion] {				
					n_completion.ExecuteIfBound(true, false, msg);
				});
				return;
			} else {
				const FString msg = FString::Printf(TEXT("Failed to query metadata for '%s' in '%s': %s"),
					*n_key.ObjectKey, *n_key.BucketName, UTF8_TO_TCHAR(client.status().message().c_str()));
				UE_LOG(LogCloudConnector, Warning, TEXT("%s"), *msg);

				Async(EAsyncExecution::TaskGraphMainThread, [msg, n_completion] {		
					n_completion.ExecuteIfBound(false, false, msg);
				});
				return;
			}
		}

		// If we got metadata, the object must exist
		const FString msg = FString::Printf(TEXT("Object '%s' exists in bucket '%s'"),
				*n_key.ObjectKey, *n_key.BucketName);
		UE_LOG(LogCloudConnector, Display, TEXT("%s"), *msg);

		Async(EAsyncExecution::TaskGraphMainThread, [msg, n_completion] {
			n_completion.ExecuteIfBound(true, true, msg);
		});
	});

	return true;
}

bool GoogleCloudStorageImpl::write(const FCloudStorageKey &n_key, const TArrayView<const uint8> n_data,
		const FCloudStorageWriteFinishedDelegate n_completion) {
	
	if (!n_data.Num()) {
		UE_LOG(LogCloudConnector, Warning, TEXT("Cannot upload empty data to Google Storage"));
		return false;
	}

	// Let's do some sanity checks on the bucket and key names
	if (!valid_google_bucket_name(n_key.BucketName)) {
		return false;
	}

	if (!valid_google_object_key(n_key.ObjectKey)) {
		return false;
	}

	// go async here and do the actual upload in the thread pool
	Async(EAsyncExecution::ThreadPool, [n_key, n_data, n_completion] {

		UE_LOG(LogCloudConnector, Display, TEXT("Starting Google Storage upload"));

		const std::string bucket_name{ TCHAR_TO_ANSI(*n_key.BucketName) };
		const std::string object_key{ TCHAR_TO_ANSI(*n_key.ObjectKey) };
		const std::string content_type{ TCHAR_TO_ANSI(*n_key.ObjectKey) };

		try {
			// Create a client to communicate with Google Cloud Storage. Clients are said to 
			// be "relatively low cost" but not threadsafe on instance level. So I create them
			// right here.
			
			gc::StatusOr<gcs::Client> client = gcs::Client::CreateDefaultClient();
			if (!client) {
				const FString msg = FString::Printf(TEXT("Failed to create client for upload of '%s' to bucket '%s': %s"),
					*n_key.ObjectKey, *n_key.BucketName, UTF8_TO_TCHAR(client.status().message().c_str()));
				UE_LOG(LogCloudConnector, Warning, TEXT("%s"), *msg);

				Async(EAsyncExecution::TaskGraphMainThread, [msg, n_completion] {
					n_completion.ExecuteIfBound(false, msg);
				});
				return;
			}
			
			// start the write op
			gcs::ObjectWriteStream writer = client->WriteObject(bucket_name, object_key, gcs::ContentType(content_type));

			// Yeah, let's hope the caller read our notice about having to maintain ownership
			// over the buffer. If it disappears during this op, we crash.
			writer.write(reinterpret_cast<const char *>(n_data.GetData()), n_data.Num());
			writer.Close();
			
			// Sync back to the game thread when done and call delegate
			if (writer.metadata()) {
				const FString msg = FString::Printf(TEXT("Successfully uploaded '%s' to bucket '%s'"),
						*n_key.ObjectKey, *n_key.BucketName);
				UE_LOG(LogCloudConnector, Display, TEXT("%s"), *msg);
				Async(EAsyncExecution::TaskGraphMainThread, [msg, n_completion] {
					n_completion.ExecuteIfBound(true, msg);
				});
			} else {
				const FString msg = FString::Printf(TEXT("Failed uploading '%s' to bucket '%s': %s"),
						*n_key.ObjectKey, *n_key.BucketName, UTF8_TO_TCHAR(writer.metadata().status().message().c_str()));
				UE_LOG(LogCloudConnector, Warning, TEXT("%s"), *msg);
				Async(EAsyncExecution::TaskGraphMainThread, [msg, n_completion] {
					n_completion.ExecuteIfBound(false, msg);
				});
			}

		// So. I'm quite unsure whether or not the library does throw
		// Since I have seen at least exceptions raising out of abseil, I think
		// it's safe to assume the worst
		} catch (const std::exception &sex) {
			const FString msg = FString::Printf(TEXT("Exception uploading '%s' to bucket '%s': %s"),
					*n_key.ObjectKey, *n_key.BucketName, UTF8_TO_TCHAR(sex.what()));
			UE_LOG(LogCloudConnector, Warning, TEXT("%s"), *msg);
			Async(EAsyncExecution::TaskGraphMainThread, [n_key, msg, n_completion] {			
				n_completion.ExecuteIfBound(false, msg);
			});		
		}
	});

	return true;
}
