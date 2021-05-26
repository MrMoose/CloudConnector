/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "CloudConnectorTypes.h"

#include <chrono>

inline long long millis_since_epoch() {

	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
}

/**
 * @brief load an environment variable
 * I have had bad experience with the engine builtin environment getters,
 * so this wraps up my own and uses _dupenv_s on windows
 * @param n_env_variable_name name of the environment variable
 * @param n_default return default value if not found
 */
FString readenv(const FString &n_env_variable_name, const FString &n_default = FString{});

/**
 * @brief encapsulate putenv as well
 * Not threadsafe.
 * @param n_env_variable_name name of the environment variable
 * @param n_value value string
 * @return true on success
 */
bool writeenv(const FString &n_env_variable_name, const FString &n_value, const bool n_overwrite);

/**
 * @brief Read env variable CLOUDCONNECTOR_CLOUD_PROVIDER to determine which impl to use
 */
ECloudProvider cloud_provider(const ECloudProvider n_default);

/**
 * @brief Read env variable CLOUDCONNECTOR_ENDPOINT_DISCOVERY_ENABLED to see if we are to use endpoint discovery
 * @return defaults to false, as is the case in AWS SDK
 */
bool use_endpoint_discovery();

/**
 * @brief Read env variable CLOUDCONNECTOR_LOGS_ENABLED to determine if we should activate CloudWatch logging
 * @return defaults to n_default, true if env set to "True", false if env set to "False"
 */
bool logs_enabled(const bool n_default);

/**
 * @brief Read env variable CLOUDCONNECTOR_TRACING_ENABLED to determine if we should activate tracing
 * @return defaults to n_default, true if env set to "True", false if env set to "False"
 */
bool tracing_enabled(const bool n_default);

/** @brief Query the AWS metadata server for the EC2 Instance ID.
 * Blocking for 3 seconds at worst case
 * If not on EC, will return "LocalInstance" after timeout
 * 
 * If the environment variable CLOUDCONNECTOR_INSTANCE_ID is set,
 * its value will be taken instead and the metadata server will not be queried
 */
FString get_aws_instance_id();

/** @brief Query the Google Cloud metadata server for the Compute Engine Instance ID.
 * Blocking for 3 seconds at worst case
 * If not on Compute Engine, will return "LocalInstance" after timeout
 * 
 * If the environment variable CLOUDCONNECTOR_INSTANCE_ID is set,
 * its value will be taken instead and the metadata server will not be queried
 */
FString get_google_cloud_instance_id();

/// generate what I think is a random trace ID for use in XRay
FString random_aws_trace_id();
