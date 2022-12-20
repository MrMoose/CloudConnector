/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "BlindTrace.h"
#include "ICloudConnector.h"
#include "Utilities.h"

#include "Modules/ModuleManager.h"


BlindTrace::BlindTrace(const FString &n_trace_id)
		: m_id{ n_trace_id } {

}

BlindTrace::~BlindTrace() {

}

ScopedSegment BlindTrace::scoped_segment(const FString &n_name) {

	return ScopedSegment{ *this, n_name };
}

const FString &BlindTrace::id() const noexcept {

	return m_id;
}

bool BlindTrace::start_segment(const FString &n_name) {

	return true;
}

bool BlindTrace::end_segment(const FString &n_name, const bool n_error /* = false*/) noexcept {
	
	return true;
}

