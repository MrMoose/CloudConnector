/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "HAL/Thread.h"
#include "Templates/Atomic.h"
#include "Logging/LogVerbosity.h"
#include "Misc/OutputDevice.h"
#include "Containers/Queue.h"

#include "Windows/PreWindowsApi.h"
#include <aws/core/Aws.h>
#include <aws/logs/CloudWatchLogsClient.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include "Windows/PostWindowsApi.h"


/** @brief logging backend for Engine logs
 *  which sends logs to AWS CloudWatch every 5 seconds
 */
class FCloudWatchLogOutputDevice : public FOutputDevice {

	public:
		FCloudWatchLogOutputDevice(const FString &n_log_group_prefix, const ELogVerbosity::Type n_min_verbosity);
		~FCloudWatchLogOutputDevice() noexcept;
		FCloudWatchLogOutputDevice(FCloudWatchLogOutputDevice &&) = delete;
		FCloudWatchLogOutputDevice(const FCloudWatchLogOutputDevice &) = delete;
		FCloudWatchLogOutputDevice &operator=(FCloudWatchLogOutputDevice &&) = delete;
		FCloudWatchLogOutputDevice &operator=(const FCloudWatchLogOutputDevice &) = delete;

		/** @defgroup FOutputDevice interface
		 *   For docs, see OutputDevice.h  ;-)
		 *   @{
		 */
		void TearDown() override;
		void Serialize(const TCHAR *n_message, ELogVerbosity::Type n_verbosity, const FName &n_category) override;
		void Serialize(const TCHAR *n_message, ELogVerbosity::Type n_verbosity, const FName &n_category, const double n_time) override;
		bool CanBeUsedOnMultipleThreads() const override;
		bool CanBeUsedOnAnyThread() const override;
		//! @}

	private:
		/// background thread that will loop and send logs as required
		void log_thread() noexcept;

		/// called from thread
		void send_log_messages() noexcept;

		// Base log group name on environment variable CLOUDCONNECTOR_STACK_NAME
		FString get_log_group_name() noexcept;

		// Find a suitable log stream name by using the instance ID
		// and then attach date stamp
		static FString get_log_stream_name(const FString &n_instance_id) noexcept;

		// AWS SDK Client object, owned and accessed by thread.
		Aws::UniquePtr<Aws::CloudWatchLogs::CloudWatchLogsClient> m_cwclient;

		struct LogEntry {
			long long             m_timestamp;   //!< millis since epoch (type required by InputLogEvent)
			Aws::String           m_message;
		};

		using LogQueue = TQueue<LogEntry, EQueueMode::Mpsc>;

		TUniquePtr<FThread>       m_log_thread;
		TAtomic<bool>             m_interrupted;

		LogQueue                  m_log_q;
		
		FString                   m_instance_id;
		const FString             m_log_group_prefix;
		const ELogVerbosity::Type m_min_verbosity;
		Aws::String               m_log_group_name;
		Aws::String               m_log_stream_name;
		Aws::String               m_upload_sequence_token;
};

