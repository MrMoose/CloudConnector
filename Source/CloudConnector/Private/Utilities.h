/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"

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
 * @brief Read env variable CLOUDCONNECTOR_ENDPOINT_DISCOVERY_ENABLED to see if we are to use endpoint discovery
 * @return defaults to false, as is the case in AWS SDK
 */
bool use_endpoint_discovery();

/**
 * @brief Read env variable CLOUDCONNECTOR_LOGS_ENABLED to determine if we should activate CloudWatch logging
 * @return defaults to n_default, true if env set to "True", false if env set to "False"
 */
bool logs_enabled(const bool n_default);

/** @brief Query the AWS metadata server for the EC2 Instance ID.
 * Blocking for 3 seconds at worst case
 * If not on EC, will return "LocalInstance" after timeout
 */
FString get_aws_instance_id();

/** @brief Query the Google Cloud metadata server for the Compute Engine Instance ID.
 * Blocking for 3 seconds at worst case
 * If not on Compute Engine, will return "LocalInstance" after timeout
 */
FString get_google_cloud_instance_id();
