/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"

struct TracePayload final {

	struct Segment {

		// to be able to use AddUnique() and such
		bool operator==(const Segment &n_rhs) const {
			return m_name.Equals(n_rhs.m_name);
		}

		FString   m_name;
		double    m_start; // UTC millis since epoch
		double    m_end;
		bool      m_error = false;
	};

	const FString        m_trace_id;
	const double         m_start = 0.0;       // Start of the entire trace, initialized by CloudTrace
	double               m_end;

	// Although inefficient for many segments, I expect only a few of them
	// but want them to be in order of appearance, which is why I chose
	// an array here for what could have been a TMap or TSet
	FCriticalSection     m_mutex;
	TArray<Segment>      m_segments;
};

using TracePayloadUPtr = TUniquePtr<TracePayload>;

/** This is your primary interface to do one trace.
 *  Use ICloudTracing::start_trace() to create one, then keep it around
 *  throughout the lifetime of you processing a request.
 *  Use ScopedSegment or start_segment() / end_segment() to trace
 *  things happening within your application.
 *  When the object goes out of scope, collected trace segments will be
 *  written to the cloud backend.
 */
class CLOUDCONNECTOR_API CloudTrace final {

	public:
		CloudTrace() = delete;
		CloudTrace(const CloudTrace &) = delete;
		~CloudTrace() noexcept;

		/// You can use this to easily scope a trace on the stack.
		/// Use scoped_segment() to create one
		class ScopedSegment final {

			public:
				ScopedSegment() = delete;
				ScopedSegment(const ScopedSegment &) = delete;
				ScopedSegment(ScopedSegment &&) = default;
				~ScopedSegment() noexcept {	
					m_parent.end_segment(m_name, m_error);
				}

				// Some backends (XRay) support a notion of an error in a trace segment.
				// Using this, you can make the segment appear as such.
				// Not relyable, impls are free to ignore this.
				// For compatibility reasons, setting this on one segment
				// will cause the entire trace to carry that fault marker.
				void set_error(const bool n_error) noexcept {
					m_error = n_error;
				}
				
			private:
				friend class CloudTrace;

				ScopedSegment(class CloudTrace &n_parent, const FString &n_name)
						: m_parent{ n_parent }
						, m_name{ n_name } {
				
					m_parent.start_segment(n_name);
				}

				class CloudTrace &m_parent;
				const FString     m_name;
				bool              m_error = false;
		};

		/** open up a new scoped segment upon this trace which will end itself upon deletion
		 *  @param n_name the human readable name of the segment
		 */ 
		ScopedSegment scoped_segment(const FString &n_name);

		/**
		 * @brief start a new trace segment with a given name.
		 * This will appear as a part of the trace. A segment. Choose a name that will
		 * tell which operation takes how long.
		 * Using the same name for multiple segments will result in undefined behavior.
		 * Note that this is thread safe on object level.
		 * @param n_name the name of your segment
		 * @return true if a new segment has been started. false if the name was rejected
		 */
		bool start_segment(const FString &n_name);

		/**
		 * @brief end a trace segment. Which means record the end time
		 * @param n_name the name you used in start_segment()
		 * @return true if the segment was propery ended, otherwise false
		*/
		bool end_segment(const FString &n_name, const bool n_error = false) noexcept;

		const FString &id() const noexcept;

	private:
		// only ICloudTracing may create such objects
		template <typename ObjectType>
		friend class SharedPointerInternals::TIntrusiveReferenceController;
		friend class ICloudTracing;
		friend class AWSTracingImpl;
		friend class GoogleTracingImpl;
		friend class BlindTracingImpl;

		CloudTrace(const FString &n_trace_id);

		// When we go out of scope, this is likely to get stolen
		// by the trace impl as it goes async to write the data.
		TracePayloadUPtr m_payload;
};

using CloudTracePtr = TSharedPtr<CloudTrace, ESPMode::ThreadSafe>;
using CloudTraceWeakPtr = TWeakPtr<CloudTrace, ESPMode::ThreadSafe>;


/** Trace mechanism to gather performance data.
 *  This maps to XRay on AWS or Google Cloud Trace.
 *  All methods are safe to be called from any thread
 */
class CLOUDCONNECTOR_API ICloudTracing {

	public:
		virtual ~ICloudTracing() = default;

		/**
		 * @brief Call this to create a new trace
		 * Can be used until the returned ptr goes out of scope.
		 * @param n_trace_id this came in via your cloud system.
		 * @return a new trace or null if your backend doesn't support it or the trace id was invalid
		*/
		virtual CloudTracePtr start_trace(const FString &n_trace_id);

		/**
		 * @brief Call this to get an existing trace
		 * You can use this to get a trace that you have reason to believe still exists.
		 * If it does, and this returns a valild ptr, you can add additional segments.
		 * @param n_trace_id this came in via your cloud system. The same you used in start_trace()
		 * @return shared ptr to your trace or null if the trace went out of scope.
		 */
		// virtual CloudTracePtr get_trace(const FString &n_trace_id);

	protected:
		friend class CloudTrace;
		virtual void finish_trace(CloudTrace &n_trace);

		virtual void write_trace_document(CloudTrace &n_trace) = 0;

		/*
		FCriticalSection                 m_mutex;
		TMap<FString, CloudTraceWeakPtr> m_open_traces;
		*/
};
