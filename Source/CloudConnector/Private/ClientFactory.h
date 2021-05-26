/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "Utilities.h"

#include "Windows/PreWindowsApi.h"
#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/utils/memory/AWSMemory.h>  // for unique_ptr
#include <aws/core/utils/memory/stl/AWSString.h>
#include "Windows/PostWindowsApi.h"


namespace Aws::CloudWatchLogs { class CloudWatchLogsClient; }
namespace Aws::SQS { class SQSClient; }
namespace Aws::S3 { class S3Client; }
namespace Aws::XRay { class XRayClient; }


/** @brief create an AWS client object according to our settings 
	and some of the old environment overideroo
 */
template<typename ClientType>
struct aws_client_factory {

	public:
		/// Get us a preconfigured Aws client object depending on type
		/// When adding new types, sadly they need to be fwd declared above
		/// just in case you're here because of that. 
		static Aws::UniquePtr<ClientType> create() {
	
			Aws::Client::ClientConfiguration config = client_config();
			config.enableEndpointDiscovery = use_endpoint_discovery();
			config.endpointOverride = TCHAR_TO_ANSI(*readenv(s_endpoint_override_env));

			// Careful tinkering with that
			const FString aws_region = readenv(TEXT("CLOUDCONNECTOR_AWS_REGION"));
			if (!aws_region.IsEmpty()) {
				config.region = TCHAR_TO_ANSI(*aws_region);
			}

			// Upon special request I want to override the SDK's behavior of taking
			// the credentials from .aws/credentials and supply them by env variable
			const FString aws_access_key = readenv(TEXT("CLOUDCONNECTOR_AWS_ACCESS_KEY"));
			const FString aws_secret_key = readenv(TEXT("CLOUDCONNECTOR_AWS_SECRET_KEY"));

			if (!aws_access_key.IsEmpty() && !aws_secret_key.IsEmpty()) {
	
				const Aws::Auth::AWSCredentials creds{
					TCHAR_TO_ANSI(*aws_access_key),
					TCHAR_TO_ANSI(*aws_secret_key)
				};
		
				return Aws::MakeUnique<ClientType>(s_memtag, creds, config);
			} else {
				return Aws::MakeUnique<ClientType>(s_memtag, config);
			}
		}

	private:

		// not sure what that is but MakeUnique of Aws requires it.
		// I assume it's got something to do with internally
		// used slab allocators. Perhaps this could be the same for every type?
		static const char *s_memtag;

		// Each type of client uses a different environment variable
		// to tell if they are to use endpoint override.
		// Specialize this to set this for a client. See below.
		static const FString s_endpoint_override_env;

		// get us a client config for that type
		static Aws::Client::ClientConfiguration client_config() {
			return {};
		};
};


template<>
const char *aws_client_factory<Aws::CloudWatchLogs::CloudWatchLogsClient>::
s_memtag = "CloudWatch";

template<>
const FString aws_client_factory<Aws::CloudWatchLogs::CloudWatchLogsClient>::
s_endpoint_override_env = FString{ TEXT("CLOUDCONNECTOR_AWS_CLOUDWATCH_ENDPOINT") };

template<>
const char *aws_client_factory<Aws::SQS::SQSClient>::
s_memtag = "Sqs";

template<>
const FString aws_client_factory<Aws::SQS::SQSClient>::
s_endpoint_override_env = FString{ TEXT("CLOUDCONNECTOR_AWS_SQS_ENDPOINT") };

// Experiments have shown very strange behavior of the SQS client
// when used with timeouts too high or poll times above 4 seconds.
// So we use somewhat tighter timeouts here
template<>
Aws::Client::ClientConfiguration aws_client_factory<Aws::SQS::SQSClient>::
client_config() {

	Aws::Client::ClientConfiguration ret;
	ret.httpRequestTimeoutMs = 7000;
	ret.requestTimeoutMs = 6000;
	return ret;
}

template<>
const char *aws_client_factory<Aws::S3::S3Client>::
s_memtag = "S3";

template<>
const FString aws_client_factory<Aws::S3::S3Client>::
s_endpoint_override_env = FString{ TEXT("CLOUDCONNECTOR_AWS_S3_ENDPOINT") };

template<>
const char *aws_client_factory<Aws::XRay::XRayClient>::
s_memtag = "XRay";

template<>
const FString aws_client_factory<Aws::XRay::XRayClient>::
s_endpoint_override_env = FString{ TEXT("CLOUDCONNECTOR_AWS_XRAY_ENDPOINT") };

