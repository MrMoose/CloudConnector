/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "Utilities.h"
#include "ICloudConnector.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/DefaultValueHelper.h"
#include "Internationalization/Regex.h"

#include <cstdlib>
#include <random>

FString readenv(const FString &n_env_variable_name, const FString &n_default) {

#ifdef _WIN32
	char *buffer = nullptr;
	size_t size = 0;
	if (_dupenv_s(&buffer, &size, TCHAR_TO_UTF8(*n_env_variable_name)) == 0 && buffer != nullptr) {
		return UTF8_TO_TCHAR(buffer);
	}

	return n_default;
#else
	if (const char *env = std::getenv(TCHAR_TO_UTF8(*n_env_variable_name))) {
		return UTF8_TO_TCHAR(env);
	}

	const n_default;
#endif
}

bool writeenv(const FString &n_env_variable_name, const FString &n_value, const bool n_overwrite) {

#ifdef _WIN32
	char *buffer = nullptr;
	size_t size = 0;
	if (_dupenv_s(&buffer, &size, TCHAR_TO_UTF8(*n_env_variable_name)) == 0 && buffer != nullptr && !n_overwrite) {
		return false;
	}

	return !_putenv_s(TCHAR_TO_UTF8(*n_env_variable_name), TCHAR_TO_UTF8(*n_value));
#else
	return !putenv(TCHAR_TO_UTF8(*n_env_variable_name), TCHAR_TO_UTF8(*n_value), n_overwrite);
#endif
}


namespace {

inline bool true_or_false_env(const FString &n_env_variable, const bool n_default = false) {
	
	const FString env{ readenv(n_env_variable) };

	if (env.Equals("True", ESearchCase::IgnoreCase)) {
		return true;
	}

	if (env.Equals("False", ESearchCase::IgnoreCase)) {
		return false;
	}

	return n_default;
}

} // anon ns

ECloudProvider cloud_provider(const ECloudProvider n_default) {

	const FString env{ readenv(TEXT("CLOUDCONNECTOR_CLOUD_PROVIDER")) };

	if (env.Equals(TEXT("AWS"))) {
		return ECloudProvider::AWS;
	}

	if (env.Equals(TEXT("Google"))) {
		return ECloudProvider::GOOGLE;
	}

	if (env.Equals(TEXT("None"))) {
		return ECloudProvider::BLIND;
	}

	return n_default;
}

bool use_endpoint_discovery() {

	return true_or_false_env(TEXT("CLOUDCONNECTOR_ENDPOINT_DISCOVERY_ENABLED"), false);
}

bool logs_enabled(const bool n_default) {

	return true_or_false_env(TEXT("CLOUDCONNECTOR_LOGS_ENABLED"), n_default);
}

bool tracing_enabled(const bool n_default) {

	return true_or_false_env(TEXT("CLOUDCONNECTOR_TRACING_ENABLED"), n_default);
}

uint32 visibility_timeout(const uint32 n_default) {

	const FString env{ readenv(TEXT("CLOUDCONNECTOR_VISIBILITY_TIMEOUT")) };
	if (env.IsEmpty()) {
		return n_default;
	}

	int32 parsed = 0;
	const bool result = FDefaultValueHelper::ParseInt(env, parsed);

	if (!result) {
		return n_default;
	} else {
		return static_cast<uint32>(parsed);
	}
}

FString google_project_id(const FString n_default) {

	const FString env{ readenv(TEXT("CLOUDCONNECTOR_GOOGLE_PROJECT_ID")) };
	if (env.IsEmpty()) {
		return n_default;
	} else {
		return n_default;
	}
}

FString get_aws_instance_id() {

	const FString env_instance_id{ readenv(TEXT("CLOUDCONNECTOR_INSTANCE_ID")) };
	if (!env_instance_id.IsEmpty()) {
		return env_instance_id;
	}

	FString instance_id{ TEXT("LocalInstance") };

	const float timeout_before_call = FHttpModule::Get().GetHttpTimeout();
	FHttpModule::Get().SetHttpTimeout(3.0);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> md_request = FHttpModule::Get().CreateRequest();
	md_request->SetVerb("GET");
	md_request->SetURL(TEXT("http://169.254.169.254/latest/meta-data/instance-id"));

	// There seems to be no sync Http request in this module. Mock it by using a future.
	TSharedPtr<TPromise<bool>, ESPMode::ThreadSafe> promise = MakeShareable<TPromise<bool> >(new TPromise<bool>());
	TFuture<bool> return_future = promise->GetFuture();

	md_request->OnProcessRequestComplete().BindLambda(
			[promise, &instance_id](FHttpRequestPtr /*Request*/, FHttpResponsePtr n_response, bool n_success) {
		if (!n_success) {
			promise->SetValue(false);
			return;
		}

		if (n_response->GetResponseCode() != 200) {
			promise->SetValue(false);
			return;
		}

		instance_id = n_response->GetContentAsString();
		promise->SetValue(true);
	});

	// This starts the Http request, return lambda will be called async, so we sit and wait
	md_request->ProcessRequest();
	return_future.WaitFor(FTimespan::FromSeconds(4.0));
	const bool result = return_future.Get();

	// This timeoout value appears to be global and I think it might be
	// best to set it back to the value it had before I touched it
	FHttpModule::Get().SetHttpTimeout(timeout_before_call);

	return instance_id;
}

// See https://cloud.google.com/compute/docs/storing-retrieving-metadata
std::string get_google_cloud_instance_id() {

	const FString env_instance_id{ readenv(TEXT("CLOUDCONNECTOR_INSTANCE_ID")) };
	if (!env_instance_id.IsEmpty()) {
		return TCHAR_TO_UTF8(*env_instance_id);
	}

	FString instance_id{ TEXT("LocalInstance") };

	const float timeout_before_call = FHttpModule::Get().GetHttpTimeout();
	FHttpModule::Get().SetHttpTimeout(3.0);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> md_request = FHttpModule::Get().CreateRequest();
	md_request->SetVerb("GET");
	md_request->AppendToHeader(TEXT("Metadata-Flavor"), TEXT("Google"));
	md_request->SetURL(TEXT("http://metadata.google.internal/computeMetadata/v1/instance/id"));

	// There seems to be no sync Http request in this module. Mock it by using a future.
	TSharedPtr<TPromise<bool>, ESPMode::ThreadSafe> promise = MakeShareable<TPromise<bool> >(new TPromise<bool>());
	TFuture<bool> return_future = promise->GetFuture();

	md_request->OnProcessRequestComplete().BindLambda(
		[promise, &instance_id](FHttpRequestPtr /*Request*/, FHttpResponsePtr n_response, bool n_success) {
			if (!n_success) {
				promise->SetValue(false);
				return;
			}

			if (n_response->GetResponseCode() != 200) {
				promise->SetValue(false);
				return;
			}

			instance_id = n_response->GetContentAsString();
			promise->SetValue(true);
		});

	// This starts the Http request, return lambda will be called async, so we sit and wait
	md_request->ProcessRequest();
	
	if (return_future.WaitFor(FTimespan::FromSeconds(4.0))) {
		// This timeoout value appears to be global and I think it might be
		// best to set it back to the value it had before I touched it
		FHttpModule::Get().SetHttpTimeout(timeout_before_call);
		return TCHAR_TO_UTF8(*instance_id);
	} else {
		FHttpModule::Get().SetHttpTimeout(timeout_before_call);
		return "LocalInstance";
	}
}

FString random_aws_trace_id() {

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0, std::numeric_limits<uint64>::max());
	return FString::Printf(TEXT("%016x"), dist(mt));
}

FString parse_arn_account_id(const FString &n_arn) {

	// Seems like I need to parse the account ID out the q arn
	FRegexPattern arn_regex(TEXT("^arn:([^:\\n]*):([^:\\n]*):([^:\\n]*):([^:\\n]*):(?:([^:\\/\\n]*)[:\\/])?(.*)$"));

	FRegexMatcher matcher(arn_regex, n_arn);

	if (!matcher.FindNext()) {
		UE_LOG(LogCloudConnector, Warning, TEXT("Cannot parse arn: %s"), *n_arn);
		return {};
	} else {
		return matcher.GetCaptureGroup(4);
	}
}

FString hacky_arn_prefix(const FString &n_topic_arn, const FString &n_queue_name) {

	// Seems like I need to parse the account ID out the q arn
	FRegexPattern arn_regex(TEXT("^arn:([^:\\n]*):([^:\\n]*):([^:\\n]*):([^:\\n]*):(?:([^:\\/\\n]*)[:\\/])?(.*)$"));

	FRegexMatcher matcher(arn_regex, n_topic_arn);

	if (!matcher.FindNext()) {
		UE_LOG(LogCloudConnector, Warning, TEXT("Cannot parse arn: %s"), *n_topic_arn);
		return {};
	} else {
		// surely I belong dead for this
		return FString::Printf(TEXT("arn:%s:sqs:%s:%s:%s"),
			*matcher.GetCaptureGroup(1),
			*matcher.GetCaptureGroup(3),
			*matcher.GetCaptureGroup(4),
			*n_queue_name);
	}
}

