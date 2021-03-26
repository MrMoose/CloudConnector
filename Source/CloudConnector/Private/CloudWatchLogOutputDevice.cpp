/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "CloudWatchLogOutputDevice.h"
#include "ICloudConnector.h"
#include "Utilities.h"

// Engine
#include "Async/AsyncWork.h"

// AWS SDK
#include "Windows/PreWindowsApi.h"
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <aws/logs/CloudWatchLogsErrors.h>
#include <aws/logs/model/PutLogEventsRequest.h>
#include <aws/logs/model/CreateLogGroupRequest.h>
#include <aws/logs/model/CreateLogStreamRequest.h>
#include "Windows/PostWindowsApi.h"

// Std
#include <string>

using namespace Aws::CloudWatchLogs::Model;
using Aws::CloudWatchLogs::CloudWatchLogsError;
using Aws::CloudWatchLogs::CloudWatchLogsErrors;

FCloudWatchLogOutputDevice::FCloudWatchLogOutputDevice(const FString &n_log_group_prefix)
		: m_interrupted{ false }
		, m_log_group_prefix(n_log_group_prefix)
		, m_log_group_name{ TCHAR_TO_UTF8(*n_log_group_prefix) } {

	bAutoEmitLineTerminator = false;
	m_log_group_name.append("unspecified");
	m_log_thread = MakeUnique<FThread>(TEXT("AWS_Logging"), [this] { this->log_thread(); });
}

FCloudWatchLogOutputDevice::~FCloudWatchLogOutputDevice() noexcept {

	TearDown();
}

void FCloudWatchLogOutputDevice::TearDown() {

	// we are running and must join
	if (m_log_thread) {
		m_interrupted.Store(true);
		m_log_thread->Join();
		m_log_thread.Reset();

		m_cwl.reset();
	}
}

void FCloudWatchLogOutputDevice::Serialize(const TCHAR *n_message, ELogVerbosity::Type n_verbosity, const FName &n_category, const double n_time) {

	Serialize(n_message, n_verbosity, n_category);
}

void FCloudWatchLogOutputDevice::Serialize(const TCHAR* n_message, ELogVerbosity::Type n_verbosity, const FName &n_category) {

	FCloudWatchLogOutputDevice::LogEntry entry;
	entry.m_timestamp = millis_since_epoch();
	entry.m_message.reserve(128);

	switch (n_verbosity) {
		case ELogVerbosity::NoLogging:
			entry.m_message = Aws::String("[NOLOGGING] (");
			break;
		case ELogVerbosity::Fatal:
			entry.m_message = Aws::String("[FATAL] (");
			break;
		case ELogVerbosity::Error:
			entry.m_message = Aws::String("[ERROR] (");
			break;
		case ELogVerbosity::Warning:
			entry.m_message = Aws::String("[WARNING] (");
			break;
		case ELogVerbosity::Display:
			entry.m_message = Aws::String("[INFO] (");
			break;
		case ELogVerbosity::Log:
			entry.m_message = Aws::String("[LOG] (");
			break;
		case ELogVerbosity::Verbose:
			entry.m_message = Aws::String("[VERBOSE] (");
			break;
		default:
			entry.m_message = Aws::String("[CATCH_ALL] (");
	}

	entry.m_message.append(TCHAR_TO_UTF8(*n_category.ToString()));
	entry.m_message.append(") ");
	entry.m_message.append(TCHAR_TO_UTF8(n_message));

	// This is thread safe.
	m_log_q.Enqueue(MoveTemp(entry));
}

bool FCloudWatchLogOutputDevice::CanBeUsedOnMultipleThreads() const {

	return true;
}

bool FCloudWatchLogOutputDevice::CanBeUsedOnAnyThread() const {

	return true;
}

FString FCloudWatchLogOutputDevice::get_log_group_name() noexcept {

	FString group_name{ m_log_group_prefix };
	group_name.Append(readenv(TEXT("CLOUDCONNECTOR_STACK_NAME"), TEXT("UnknownStack")));
	return group_name;
}

FString FCloudWatchLogOutputDevice::get_log_stream_name(const FString &n_instance_id) noexcept {

	const FDateTime now = FDateTime::Now();

	// chose the same name as MVAWS here for possible future replacement

	return FString::Printf(TEXT("%04i/%02i/%02i-%02i/%02i-%s"),
		now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), *n_instance_id);
}

void FCloudWatchLogOutputDevice::log_thread() noexcept {

	// We're in a new thread here and can block a while so let's get 
	// our instance ID. Weirdly, the SDK doesn't have code for that.
	m_instance_id = get_aws_instance_id();

	// instance id plus prefix
	m_log_group_name = TCHAR_TO_UTF8(*get_log_group_name());

	// timestamp ++
	m_log_stream_name = TCHAR_TO_UTF8(*get_log_stream_name(m_instance_id));

	Aws::Client::ClientConfiguration config;
	config.enableEndpointDiscovery = use_endpoint_discovery();
	const FString endpoint_override = readenv(TEXT("CLOUDCONNECTOR_CLOUDWATCH_ENDPOINT"));
	if (!endpoint_override.IsEmpty()) {
		config.endpointOverride = TCHAR_TO_UTF8(*endpoint_override);
	}
	m_cwl = Aws::MakeUnique<Aws::CloudWatchLogs::CloudWatchLogsClient>("CloudWatchLogs", config);

	// Now we should have all the data to create a log group and stream for us
	CreateLogGroupRequest clgr;
	clgr.SetLogGroupName(m_log_group_name);
	CreateLogGroupOutcome lgoc = m_cwl->CreateLogGroup(clgr);
	if (!lgoc.IsSuccess()) {
		const CloudWatchLogsError &err{ lgoc.GetError() };
		if (err.GetErrorType() != CloudWatchLogsErrors::RESOURCE_ALREADY_EXISTS) {
			UE_LOG(LogCloudConnector, Warning, TEXT("Failed to create cloudwatch log group: %s"), UTF8_TO_TCHAR(err.GetMessage().c_str()));
			return;
		}
	}

	CreateLogStreamRequest clsr;
	clsr.SetLogGroupName(m_log_group_name);
	clsr.SetLogStreamName(m_log_stream_name);
	CreateLogStreamOutcome oc = m_cwl->CreateLogStream(clsr);
	if (!oc.IsSuccess()) {
		UE_LOG(LogCloudConnector, Error, TEXT("Failed to create cloudwatch log stream: %s"), UTF8_TO_TCHAR(oc.GetError().GetMessage().c_str()));
		return;
	}

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

void FCloudWatchLogOutputDevice::send_log_messages() noexcept {

	int max_log_events = 42;

	PutLogEventsRequest request;
	if (!m_upload_sequence_token.empty()) {
		request.SetSequenceToken(m_upload_sequence_token);
	}
	request.SetLogGroupName(m_log_group_name);
	request.SetLogStreamName(m_log_stream_name);

	// One by one, pop log entries from the q and add them to the CloudWatch
	// request. Not more than 42 to not overload the body of the request.
	FCloudWatchLogOutputDevice::LogEntry entry;

	while (m_log_q.Dequeue(entry) && max_log_events--) {
		InputLogEvent ile;
		ile.SetTimestamp(entry.m_timestamp);
		ile.SetMessage(entry.m_message);
		request.AddLogEvents(std::move(ile));
	}

	// Send the logs to CloudWatch.
	PutLogEventsOutcome oc = m_cwl->PutLogEvents(request);

	if (oc.IsSuccess()) {
		m_upload_sequence_token = oc.GetResult().GetNextSequenceToken();
	} else {
		const CloudWatchLogsError &error{ oc.GetError() };
		// It may not be such a good idea to log in here. But during development I need to know
		UE_LOG(LogCloudConnector, Error, TEXT("Failed to send CloudWatch Logs: %s"), UTF8_TO_TCHAR(error.GetMessage().c_str()));
	}
}
