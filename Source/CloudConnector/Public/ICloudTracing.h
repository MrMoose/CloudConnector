/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"

#include "Templates/SharedPointer.h"


 /// You can use this to easily scope a trace on the stack.
 /// Use scoped_segment() to create one
class CLOUDCONNECTOR_API ScopedSegment final {

	public:
		ScopedSegment() = delete;
		ScopedSegment(const ScopedSegment &) = delete;
		ScopedSegment(ScopedSegment &&) = default;
		~ScopedSegment() noexcept;

		// Some backends (XRay) support a notion of an error in a trace segment.
		// Using this, you can make the segment appear as such.
		// Not relyable, impls are free to ignore this.
		// For compatibility reasons, setting this on one segment
		// will cause the entire trace to carry that fault marker.
		void set_error(const bool n_error) noexcept;

	private:
		friend class ICloudTrace;
		friend class AWSTrace;
#ifdef WITH_GOOGLECLOUD_SDK
		friend class OpenTelemetryTrace;
#endif
		friend class BlindTrace;
		ScopedSegment(ICloudTrace &n_parent, const FString &n_name);

		ICloudTrace    &m_parent;
		const FString   m_name;
		bool            m_error = false;
};


/**
 *  This is your primary interface to do one trace.
 *  Use ICloudTracing::start_trace() to create one, then keep it around
 *  throughout the lifetime of you processing a request.
 *  Use ScopedSegment or start_segment() / end_segment() to trace
 *  things happening within your application.
 *  When the object goes out of scope, collected trace segments will be
 *  written to the cloud backend.
 */
class CLOUDCONNECTOR_API ICloudTrace : public TSharedFromThis<ICloudTrace, ESPMode::ThreadSafe> {

	public:
		virtual ~ICloudTrace() noexcept;

		/** open up a new scoped segment upon this trace which will end itself upon deletion
		 *  @param n_name the human readable name of the segment
		 */
		virtual ScopedSegment scoped_segment(const FString &n_name) = 0;

		/**
		 * @brief start a new trace segment with a given name.
		 * This will appear as a part of the trace. A segment. Choose a name that will
		 * tell which operation takes how long.
		 * Using the same name for multiple segments will result in undefined behavior.
		 * Note that this is thread safe on object level.
		 * @param n_name the name of your segment
		 * @return true if a new segment has been started. false if the name was rejected
		 */
		virtual bool start_segment(const FString &n_name) = 0;

		/**
		 * @brief end a trace segment. Which means record the end time
		 * @param n_name the name you used in start_segment()
		 * @return true if the segment was propery ended, otherwise false
		 */
		virtual bool end_segment(const FString &n_name, const bool n_error = false) noexcept = 0;

		/// Get the id this segment was created with
		virtual const FString &id() const noexcept = 0;
};



using ICloudTracePtr     = TSharedPtr<ICloudTrace, ESPMode::ThreadSafe>;
using ICloudTraceWeakPtr = TWeakPtr<ICloudTrace, ESPMode::ThreadSafe>;
using ICloudTraceRef     = TSharedRef<ICloudTrace, ESPMode::ThreadSafe>;


/** 
 * Trace mechanism to gather performance data.
 * This maps to XRay on AWS or Google Cloud Trace.
 * All methods are safe to be called from any thread
 */
class CLOUDCONNECTOR_API ICloudTracing {

	public:
		virtual ~ICloudTracing() = default;

		/**
		 * @brief Call this to create a new trace
		 * Can be used until the returned ptr goes out of scope.
		 * 
		 * Note that for OpenTelemetry this trace ID must be in the form 
		 * "4F11ADB120153241965323FEA1276321", 16 HEX bytes
		 * 
		 * @param n_trace_id this came in via your cloud system.
		 * @return a new trace or null if your backend doesn't support it or the trace id was invalid
		*/
		virtual ICloudTracePtr start_trace(const FString &n_trace_id) = 0;

	protected:
		friend class AWSTrace;
#ifdef WITH_GOOGLECLOUD_SDK
		friend class OpenTelemetryTrace;
#endif
		friend class BlindTrace;

		virtual void finish_trace(ICloudTrace *n_trace) = 0;
};
