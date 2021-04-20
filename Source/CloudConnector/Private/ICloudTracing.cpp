/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "ICloudTracing.h"
#include "ICloudConnector.h"
#include "Modules/ModuleManager.h"

#include <chrono>

namespace {
	inline double epoch_millis() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
	}
}

CloudTrace::CloudTrace(const FString &n_trace_id)
		: m_payload{ MakeUnique<TracePayload>() } {
	
	const_cast<FString &>(m_payload->m_trace_id) = n_trace_id;
	const_cast<double &>(m_payload->m_start)  = epoch_millis();
}

CloudTrace::~CloudTrace() {

	static const FName CCModuleName = "CloudConnector";

	if (FModuleManager::Get().IsModuleLoaded(CCModuleName)) {
		ICloudConnector::Get().tracing().finish_trace(*this);
	}
}

CloudTrace::ScopedSegment CloudTrace::scoped_segment(const FString &n_name) {

	return ScopedSegment{ *this, n_name };
}

const FString &CloudTrace::id() const noexcept {

	return m_payload->m_trace_id;
}

bool CloudTrace::start_segment(const FString &n_name) {

	// probably check name constraints once we know if:
	//  a) there is a reasonable impl agnostic rule
	//  b) it is worth it

	TracePayload::Segment new_segment;
	new_segment.m_name = n_name;
	new_segment.m_start = epoch_millis();

	FScopeLock slock(&m_payload->m_mutex);
	m_payload->m_segments.AddUnique(MoveTemp(new_segment));

	return true;
}

bool CloudTrace::end_segment(const FString &n_name, const bool n_error /* = false*/) noexcept {

	const double now = epoch_millis();

	FScopeLock slock(&m_payload->m_mutex);
	TracePayload::Segment * const segment = m_payload->m_segments.FindByPredicate([&n_name] (const TracePayload::Segment &s) {
		return s.m_name.Equals(n_name);
	});

	// Now I really expect the segment to be there but can't get myself 
	// to checkf() for this as I would be asserting the user's behavior, not mine.
	if (!segment) {
		return false;
	}

	segment->m_end = now;
	segment->m_error = n_error;

	return true;
}

CloudTracePtr ICloudTracing::start_trace(const FString &n_trace_id) {

	if (n_trace_id.IsEmpty()) {
		return {};
	}

	CloudTracePtr new_trace = MakeShared<CloudTrace, ESPMode::ThreadSafe>(n_trace_id);

	// FScopeLock slock(&m_mutex);
	// m_open_traces.Emplace(n_trace_id, new_trace);
	return new_trace;
}

/*
CloudTracePtr ICloudTracing::get_trace(const FString &n_trace_id) {

	if (n_trace_id.IsEmpty()) {
		return {};
	}

	FScopeLock slock(&m_mutex);
	CloudTraceWeakPtr *t = m_open_traces.Find(n_trace_id);

	if (t && t->IsValid()) {
		return t->Pin();
	}

	return {};
}
*/

void ICloudTracing::finish_trace(CloudTrace &n_trace) {

	// We check if the user has forgotten to close any open segments.
	// I don't want this in the serialization
	for (TracePayload::Segment &s : n_trace.m_payload->m_segments) {
		if (s.m_end < 1.0) {
			s.m_end = epoch_millis();
		}
	}

	n_trace.m_payload->m_end = epoch_millis();

	/*
	{
		FScopeLock slock(&m_mutex);
		m_open_traces.Remove(n_trace.id());
	}
	*/

	return write_trace_document(n_trace);
}
