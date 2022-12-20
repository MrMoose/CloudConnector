/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudTracing.h"

#include "Misc/TVariant.h"


// To enable scoped trace macros, this mocks up one that can be created 
// without an actual trace. It just logs
class CLOUDCONNECTOR_API BlindScopedSegment final {

	public:
		BlindScopedSegment();
		BlindScopedSegment(const FString &n_name);
		BlindScopedSegment(const BlindScopedSegment &) = delete;
		BlindScopedSegment(BlindScopedSegment &&) = default;
		~BlindScopedSegment() noexcept;
		void set_error(const bool n_error) noexcept;
	private:
		const bool      m_initialized;
		const FString   m_segment_name;
		const FDateTime m_start;
		bool            m_error = false;
};

#define CC_START_TRACE_SEGMENT(trace, name)       \
if (trace) {                                      \
	trace->start_segment(name);                   \
}

#define CC_END_TRACE_SEGMENT_OK(trace, name)      \
if (trace) {                                      \
	trace->end_segment(name, false);              \
}

#define CC_END_TRACE_SEGMENT_ERROR(trace, name)   \
if (trace) {                                      \
	trace->end_segment(name, true);               \
}

/* I am not a fan of such macros. Use them at your own risk
 * but beware, you cannot have two overlapping scoped segments
 * because they have the same symbol name
 */
using ScopedTraceVariant = TVariant<BlindScopedSegment, ScopedSegment>;

#define CC_SCOPED_TRACE_SEGMENT(trace, name)                          \
ScopedTraceVariant scoped_trace_segment_obfuscated_name;              \
if (trace) {                                                          \
	scoped_trace_segment_obfuscated_name.Emplace<ScopedSegment>(trace->scoped_segment(name));  \
} else {                                                              \
	scoped_trace_segment_obfuscated_name.Emplace<BlindScopedSegment>(name); \
}

#define CC_SCOPED_TRACE_SET_ERROR_FLAG()                              \
switch (scoped_trace_segment_obfuscated_name.GetIndex()) {            \
	default:                                                          \
	case 0:                                                           \
		scoped_trace_segment_obfuscated_name.Get<BlindScopedSegment>().set_error(true);  \
		break;                                                        \
	case 1:                                                           \
		scoped_trace_segment_obfuscated_name.Get<ScopedSegment>().set_error(true); \
		break;                                                        \
}

