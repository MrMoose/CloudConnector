/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "AWSPubsubImpl.h"
#include "ICloudConnector.h"

#include "Async/Async.h"
#include "Internationalization/Regex.h"

 // AWS SDK
#include "Windows/PreWindowsApi.h"
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/client/AWSError.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/PutObjectResult.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/ListObjectsResult.h>
#include "Windows/PostWindowsApi.h"

// std
#include <iostream>
#include <memory>
#include <strstream>
#include <fstream>

using namespace Aws::S3::Model;

FSubscription AWSPubsubImpl::subscribe(const FString &n_topic, const FPubsubMessageReceived n_handler) {

	return {};
}