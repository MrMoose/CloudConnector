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

// Google Cloud SDK
#pragma warning(push)
#pragma warning(disable:4583)
#pragma warning(disable:4582)

// Google Cloud SDK. Surprisingly few headers
#include "Windows/PreWindowsApi.h"
#include "google/cloud/logging/logging_service_v2_client.h"
#include "Windows/PostWindowsApi.h"
#pragma warning(pop)

// Std
#include <string>
#include <memory>

namespace gc = google::cloud;
namespace gcl = google::cloud::logging;

FGoogleLoggingOutputDevice::FGoogleLoggingOutputDevice(const FString &n_project_id)
		: m_interrupted{ false }
		, m_project_id{ n_project_id } {

	bAutoEmitLineTerminator = false;
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
	}
}

void FGoogleLoggingOutputDevice::Serialize(const TCHAR *n_message, ELogVerbosity::Type n_verbosity, const FName &n_category, const double n_time) {
	
	Serialize(n_message, n_verbosity, n_category);
}

void FGoogleLoggingOutputDevice::Serialize(const TCHAR* n_message, ELogVerbosity::Type n_verbosity, const FName &n_category) {

	FGoogleLoggingOutputDevice::LogEntry entry;
	const FDateTime now = FDateTime::UtcNow();
	entry.m_timestamp_seconds = now.ToUnixTimestamp();
	// Google only accepts nanos as next smaller fraction but we only get millis from this. So....
	entry.m_timestamp_nanos = now.GetMillisecond() * 1000 * 1000;
	entry.m_severity = n_verbosity;
	entry.m_message = TCHAR_TO_UTF8(n_message);

	// This is thread safe.
	m_log_q.Enqueue(MoveTemp(entry));
}

bool FGoogleLoggingOutputDevice::CanBeUsedOnMultipleThreads() const {

	return true;
}

bool FGoogleLoggingOutputDevice::CanBeUsedOnAnyThread() const {

	return true;
}

std::string FGoogleLoggingOutputDevice::get_log_name() noexcept {

	/* Docs

	@param log_name  Optional.A default log resource name that is assigned to
		*all log entries in `entries` that do not specify a value for `log_name`:
		* "projects/[PROJECT_ID]/logs/[LOG_ID]"
		* "organizations/[ORGANIZATION_ID]/logs/[LOG_ID]"
		* "billingAccounts/[BILLING_ACCOUNT_ID]/logs/[LOG_ID]"
		* "folders/[FOLDER_ID]/logs/[LOG_ID]"
		* `[LOG_ID]` must be URL - encoded.For example :
		*"projects/my-project-id/logs/syslog"
			* "organizations/1234567890/logs/cloudresourcemanager.googleapis.com%2Factivity"
	*/

	const std::string instance_id{ get_google_cloud_instance_id() };

	const FDateTime now = FDateTime::Now();

	const FString log_id = FString::Printf(TEXT("%04i%02i%02i-%02i%02i-%s"),
		now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), UTF8_TO_TCHAR(instance_id.c_str()));

	return std::string{ "projects/" } + TCHAR_TO_UTF8(*m_project_id.ToLower()) + "/logs/" + TCHAR_TO_UTF8(*log_id);
}

// Something defines "ERROR" as 0 in the global ns
// Need to get rid of it
#pragma push_macro("ERROR")
#undef ERROR

google::logging::type::LogSeverity to_gcl_severity(ELogVerbosity::Type n_severity) {

	switch (n_severity) {
		case ELogVerbosity::NoLogging:
			return google::logging::type::LogSeverity::EMERGENCY;
		case ELogVerbosity::Fatal:
			return google::logging::type::LogSeverity::CRITICAL;
		case ELogVerbosity::Error:
			return google::logging::type::LogSeverity::ERROR;
		case ELogVerbosity::Warning:
			return google::logging::type::LogSeverity::WARNING;
		case ELogVerbosity::Display:
			return google::logging::type::LogSeverity::DEFAULT;
		case ELogVerbosity::Log:
			return google::logging::type::LogSeverity::INFO;
		case ELogVerbosity::Verbose:
			return google::logging::type::LogSeverity::DEBUG;
		default:
			return google::logging::type::LogSeverity::INFO;
	}
}

#pragma pop_macro("ERROR")


void FGoogleLoggingOutputDevice::log_thread() noexcept {

	m_log_name = get_log_name();

	// prepare client object for google
	// This looks quite different than the other clients. I assume this is the case because
	// it's not 'really' supported..

	// This appears to be a description of what I log from. Seems to be static throughout the calls
	google::api::MonitoredResource resource;
	resource.set_type("gce_instance");
	resource.mutable_labels()->insert({ "instance_id", m_instance_id });

	std::shared_ptr<gcl::LoggingServiceV2Connection> connection{ gcl::MakeLoggingServiceV2Connection() };

	gcl::LoggingServiceV2Client client{ connection };

	std::map<std::string, std::string> labels;
	labels["WrittenBy"] = "CloudConnector";

	while (!m_interrupted) {
		// We blocked at least a little since we started this thread so send right away.
		if (!m_log_q.IsEmpty()) {

			// There seems to be a much higher limit than AWS but 
			// I will use the same semantics just in case
			int max_log_events = 42;
			
			std::vector<google::logging::v2::LogEntry> entries;
			FGoogleLoggingOutputDevice::LogEntry entry;

			while (m_log_q.Dequeue(entry) && max_log_events--) {

				google::logging::v2::LogEntry e = google::logging::v2::LogEntry::default_instance();
				e.set_severity(to_gcl_severity(entry.m_severity));
				e.mutable_timestamp()->set_seconds(entry.m_timestamp_seconds);
				e.mutable_timestamp()->set_nanos(entry.m_timestamp_nanos);
				e.set_text_payload(entry.m_message);		
				entries.emplace_back(std::move(e));
			}

			gc::StatusOr<google::logging::v2::WriteLogEntriesResponse> response = 
					client.WriteLogEntries(m_log_name, resource, labels, entries);

			if (!response.ok()) {
				const gc::Status stat = response.status();
				// What can I do on error? Log? ;-)
				// I can at least try
				UE_LOG(LogCloudConnector, Warning, TEXT("Failed to write log entries: %s"), UTF8_TO_TCHAR(stat.message().c_str()));
			} else {
				const google::logging::v2::WriteLogEntriesResponse res = response.value();
				// I don't really need that, perhaps for debugging 
			}
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

#endif // WITH_GOOGLECLOUD_SDK
