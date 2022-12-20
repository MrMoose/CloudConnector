/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "AWSMetricsImpl.h"
#include "ICloudConnector.h"
#include "TraceMacros.h"
#include "ClientFactory.h"

#include "Async/Async.h"
#include "Internationalization/Regex.h"

 // AWS SDK
#include "Windows/PreWindowsApi.h"
#include <aws/core/auth/AWSAuthSigner.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/client/AWSError.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>
#include "Windows/PostWindowsApi.h"

// std
#include <iostream>
#include <memory>
#include <strstream>
#include <fstream>

AWSMetricsImpl::AWSMetricsImpl(const ACloudConnector *n_config) {

}

