/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "AWSTrace.h"
#include "ICloudConnector.h"
#include "Utilities.h"

#include "Modules/ModuleManager.h"

#include <chrono>


AWSTrace::AWSTrace(const FString &n_trace_id)
		: m_payload{ MakeUnique<TracePayload>() } {
	
	const_cast<FString &>(m_payload->m_trace_id) = n_trace_id;
	const_cast<double &>(m_payload->m_start)  = epoch_millis();
}

AWSTrace::~AWSTrace() {

	static const FName CCModuleName = "CloudConnector";

	if (FModuleManager::Get().IsModuleLoaded(CCModuleName)) {
		ICloudConnector::Get().tracing().finish_trace(this);
	}
}

ScopedSegment AWSTrace::scoped_segment(const FString &n_name) {

	return ScopedSegment{ *this, n_name };
}

const FString &AWSTrace::id() const noexcept {

	return m_payload->m_trace_id;
}

bool AWSTrace::start_segment(const FString &n_name) {

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

bool AWSTrace::end_segment(const FString &n_name, const bool n_error /* = false*/) noexcept {

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

