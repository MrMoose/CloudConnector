/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */

#ifdef WITH_GOOGLECLOUD_SDK

#include "GoogleLoggingOutputDevice.h"
#include "ICloudConnector.h"
#include "Utilities.h"

// Engine
#include "Async/AsyncWork.h"

// Google Cloud SDK, well, not yet.

#pragma warning(push)
#pragma warning(disable:4583)
#pragma warning(disable:4582)

// Google Cloud SDK. Surprisingly few headers
#include "Windows/PreWindowsApi.h"
//#include "google/
#include "Windows/PostWindowsApi.h"
#pragma warning(pop)

// Std
#include <string>


FGoogleLoggingOutputDevice::FGoogleLoggingOutputDevice(const FString &n_log_group_prefix)
		: m_interrupted{ false }
		, m_log_group_prefix(n_log_group_prefix)
		, m_log_group_name{ TCHAR_TO_UTF8(*n_log_group_prefix) } {

	bAutoEmitLineTerminator = false;
	m_log_group_name.append("unspecified");
	m_log_thread = MakeUnique<FThread>(TEXT("Google_Logging"), [this] { this->log_thread(); });
}

FGoogleLoggingOutputDevice::~FGoogleLoggingOutputDevice() noexcept {

	TearDown();
}

void FGoogleLoggingOutputDevice::TearDown() {

	// we are running and must join
	if (m_log_thread) {
		m_interrupted.Store(true);
		m_log_thread->Join();
		m_log_thread.Reset();

		//m_cwclient.reset();
	}
}

void FGoogleLoggingOutputDevice::Serialize(const TCHAR *n_message, ELogVerbosity::Type n_verbosity, const FName &n_category, const double n_time) {
	
	// blind impl until SDK is ready
	return;

	Serialize(n_message, n_verbosity, n_category);
}

void FGoogleLoggingOutputDevice::Serialize(const TCHAR* n_message, ELogVerbosity::Type n_verbosity, const FName &n_category) {

	// blind impl until SDK is ready
	return;


	FGoogleLoggingOutputDevice::LogEntry entry;
	entry.m_timestamp = millis_since_epoch();
	entry.m_message.reserve(128);

	switch (n_verbosity) {
		case ELogVerbosity::NoLogging:
			entry.m_message = "[NOLOGGING] (";
			break;
		case ELogVerbosity::Fatal:
			entry.m_message = "[FATAL] (";
			break;
		case ELogVerbosity::Error:
			entry.m_message = "[ERROR] (";
			break;
		case ELogVerbosity::Warning:
			entry.m_message = "[WARNING] (";
			break;
		case ELogVerbosity::Display:
			entry.m_message = "[INFO] (";
			break;
		case ELogVerbosity::Log:
			entry.m_message = "[LOG] (";
			break;
		case ELogVerbosity::Verbose:
			entry.m_message = "[VERBOSE] (";
			break;
		default:
			entry.m_message = "[CATCH_ALL] (";
	}

	entry.m_message.append(TCHAR_TO_UTF8(*n_category.ToString()));
	entry.m_message.append(") ");
	entry.m_message.append(TCHAR_TO_UTF8(n_message));

	// This is thread safe.
	m_log_q.Enqueue(MoveTemp(entry));
}

bool FGoogleLoggingOutputDevice::CanBeUsedOnMultipleThreads() const {

	return true;
}

bool FGoogleLoggingOutputDevice::CanBeUsedOnAnyThread() const {

	return true;
}

FString FGoogleLoggingOutputDevice::get_log_group_name() noexcept {

	FString group_name{ m_log_group_prefix };
	group_name.Append(readenv(TEXT("CLOUDCONNECTOR_STACK_NAME"), TEXT("UnknownStack")));
	return group_name;
}

FString FGoogleLoggingOutputDevice::get_log_stream_name(const FString &n_instance_id) noexcept {

	const FDateTime now = FDateTime::Now();

	// chose the same name as MVAWS here for possible future replacement

	return FString::Printf(TEXT("%04i/%02i/%02i-%02i/%02i-%s"),
		now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), *n_instance_id);
}

void FGoogleLoggingOutputDevice::log_thread() noexcept {

	// We're in a new thread here and can block a while so let's get 
	// our instance ID. Weirdly, the SDK doesn't have code for that.
	m_instance_id = get_aws_instance_id();

	// instance id plus prefix
	m_log_group_name = TCHAR_TO_UTF8(*get_log_group_name());

	// timestamp ++
	m_log_stream_name = TCHAR_TO_UTF8(*get_log_stream_name(m_instance_id));


	// prepare client object for google


	while (!m_interrupted) {
		// We blocked at least a little since we started this thread so send right away.
		if (!m_log_q.IsEmpty()) {
			send_log_messages();
		}

		// CloudWatch requests are expensive.
		// Chose a long logging delay here in order to not send too often.
		unsigned int sleep_time = 10;
		while (sleep_time--) {
			if (m_interrupted) {
				return;
			} else {
				FPlatformProcess::Sleep(0.5);
			}
		}
	}
}

void FGoogleLoggingOutputDevice::send_log_messages() noexcept {

	int max_log_events = 42;

	// prepare request

	// One by one, pop log entries from the q and add them to the CloudWatch
	// request. Not more than 42 to not overload the body of the request.
	FGoogleLoggingOutputDevice::LogEntry entry;

	while (m_log_q.Dequeue(entry) && max_log_events--) {
		

		// request.AddLogEvents(std::move(ile));
	}

	// Send request
	
}

#endif // WITH_GOOGLECLOUD_SDK
