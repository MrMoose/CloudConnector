/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "TraceMacros.h"
#include "ICloudConnector.h"

BlindScopedSegment::BlindScopedSegment()
		: m_initialized{ false }
		, m_segment_name{ TEXT("ununitialized") }
		, m_start(FDateTime::UtcNow()) {
};

BlindScopedSegment::BlindScopedSegment(const FString &n_name)
		: m_initialized{ true }
		, m_segment_name{ n_name }
		, m_start(FDateTime::UtcNow()) {
};

BlindScopedSegment::~BlindScopedSegment() noexcept {

	if (!m_initialized) {
		return;
	}

	const FTimespan duration = FDateTime::UtcNow() - m_start;
	if (m_error) {
		UE_LOG(LogCloudConnector, Display, TEXT("Traced segment '%s' ended with an error after %f milliseconds"),
			*m_segment_name, duration.GetTotalMilliseconds());
	} else {
		UE_LOG(LogCloudConnector, Display, TEXT("Traced segment '%s' ended after %f milliseconds"),
			*m_segment_name, duration.GetTotalMilliseconds());
	}
};

void BlindScopedSegment::set_error(const bool n_error) noexcept {

	m_error = n_error;
}
