/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "ICloudTracing.h"
#include "ICloudConnector.h"
#include "Modules/ModuleManager.h"


ScopedSegment::ScopedSegment(ICloudTrace &n_parent, const FString &n_name)
		: m_parent{ n_parent }
		, m_name{ n_name } {

	m_parent.start_segment(n_name);
}

ScopedSegment::~ScopedSegment() noexcept {

	m_parent.end_segment(m_name, m_error);
}

void ScopedSegment::set_error(const bool n_error) noexcept {

	m_error = n_error;
}

ICloudTrace::~ICloudTrace() noexcept {

}

