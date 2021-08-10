/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#ifdef WITH_GOOGLECLOUD_SDK

#include "CoreMinimal.h"
#include "HAL/Thread.h"
#include "Templates/Atomic.h"
#include "Logging/LogVerbosity.h"
#include "Misc/OutputDevice.h"
#include "Containers/Queue.h"

#include <string>
#include <memory>


/** @brief logging backend for Engine logs which sends logs to Google Cloud every 5 seconds.
	It will send logs into the following name:

	"projects/$LOWERCASED_PROJECT_ID/logs/$DATE-$INSTANCEID;
 */
class FGoogleLoggingOutputDevice : public FOutputDevice {

	public:
		FGoogleLoggingOutputDevice(const FString &n_project_id);
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
		/// Cannot stick any closer to what AWS does as there appear to be stricter rules about how
		/// a log name is supposed to look like. Perhaps this is a wrong impression but I was doing it
		/// wrong but the docs are quite specific about that
		std::string get_log_name() noexcept;

		/// background thread that will loop and send logs as required
		void log_thread() noexcept;

		struct LogEntry {
			ELogVerbosity::Type  m_severity;
			int64                m_timestamp_seconds;
			int64                m_timestamp_nanos;
			std::string          m_message;
		};

		using LogQueue = TQueue<LogEntry, EQueueMode::Mpsc>;

		TUniquePtr<FThread>      m_log_thread;
		TAtomic<bool>            m_interrupted;

		LogQueue                 m_log_q;
		std::string              m_instance_id;
		const FString            m_project_id;
		std::string              m_log_name;     //!< assembled as logging starts
};

#endif // WITH_GOOGLECLOUD_SDK
