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

#include <string>

/** @brief logging backend for Engine logs
 *  which sends logs to Google Cloud every 5 seconds.
 *  This is still empty as the Google Cloud C++ SDK logging
 *  appears to be unavailable still (1.26.1).
 *  It does look like it will be at some point, which is why
 *  I left that skeleton here to ease integration later.
 */
class FGoogleLoggingOutputDevice : public FOutputDevice {

	public:
		FGoogleLoggingOutputDevice(const FString &n_log_group_prefix);
		~FGoogleLoggingOutputDevice() noexcept;
		FGoogleLoggingOutputDevice(FGoogleLoggingOutputDevice &&) = delete;
		FGoogleLoggingOutputDevice(const FGoogleLoggingOutputDevice &) = delete;
		FGoogleLoggingOutputDevice &operator=(FGoogleLoggingOutputDevice &&) = delete;
		FGoogleLoggingOutputDevice &operator=(const FGoogleLoggingOutputDevice &) = delete;

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

		struct LogEntry {
			long long            m_timestamp;   //!< millis since epoch (type required by InputLogEvent)
			std::string          m_message;
		};

		using LogQueue = TQueue<LogEntry, EQueueMode::Mpsc>;

		TUniquePtr<FThread>      m_log_thread;
		TAtomic<bool>            m_interrupted;

		LogQueue                 m_log_q;
		FString                  m_instance_id;
		const FString            m_log_group_prefix;
		std::string              m_log_group_name;
		std::string              m_log_stream_name;
		std::string              m_upload_sequence_token;
};

