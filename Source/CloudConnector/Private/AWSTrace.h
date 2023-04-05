/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudTracing.h"


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

// for docs see base class
class AWSTrace final : public ICloudTrace {

	public:
		AWSTrace() = delete;
		AWSTrace(const AWSTrace &) = delete;
		~AWSTrace() noexcept;

		ScopedSegment scoped_segment(const FString &n_name) override;
		bool start_segment(const FString &n_name) override;
		bool end_segment(const FString &n_name, const bool n_error = false) noexcept override;

		const FString &id() const noexcept override;

	private:
		// only ICloudTracing may create such objects
		template <typename ObjectType, ESPMode Mode>
		friend class SharedPointerInternals::TIntrusiveReferenceController;
		friend class ICloudTracing;
		friend class AWSTracingImpl;

		AWSTrace(const FString &n_trace_id);

		// When we go out of scope, this is likely to get stolen
		// by the trace impl as it goes async to write the data.
		TracePayloadUPtr m_payload;
};
