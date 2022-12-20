/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#ifdef WITH_GOOGLECLOUD_SDK

#include "OpenTelemetryTrace.h"
#include "ICloudConnector.h"
#include "Utilities.h"

#include "Modules/ModuleManager.h"

#pragma warning( push )
#pragma warning( disable : 4668 )

#include <opentelemetry/trace/span.h>
#include <opentelemetry/sdk/version/version.h>
#include <opentelemetry/trace/provider.h>

#pragma warning( pop )

#include <string>

namespace trace = opentelemetry::trace;
namespace nostd = opentelemetry::nostd;

OpenTelemetryTrace::OpenTelemetryTrace(const FString &n_trace_id, 
				nostd::shared_ptr<trace::Tracer> n_tracer, trace::StartSpanOptions &&n_options)
		: m_id{ n_trace_id }
		, m_tracer{ n_tracer }
		, m_outer_span{ m_tracer->StartSpan("CloudConnector", n_options) }
		, m_outer_scope{ m_tracer->WithActiveSpan(m_outer_span) } {

}

OpenTelemetryTrace::~OpenTelemetryTrace() {

	m_outer_span->End();

	static const FName CCModuleName = "CloudConnector";

	if (FModuleManager::Get().IsModuleLoaded(CCModuleName)) {
		ICloudConnector::Get().tracing().finish_trace(this);
	}
}

ScopedSegment OpenTelemetryTrace::scoped_segment(const FString &n_name) {

	return ScopedSegment{ *this, n_name };
}

const FString &OpenTelemetryTrace::id() const noexcept {

	return m_id;
}

OpenTelemetryTrace::InnerSegment::InnerSegment(opentelemetry::trace::Tracer &n_tracer, const FString &n_name)
		: m_inner_span{ n_tracer.StartSpan(TCHAR_TO_UTF8(*n_name)) }
		, m_inner_scope{ n_tracer.WithActiveSpan(m_inner_span) } {

}

OpenTelemetryTrace::InnerSegment::~InnerSegment() {

	// Even though this gets constructed and moved into the scopes map
	// the d'tor of the moved-away object is still being called.
	// So I have to check this
	if (m_inner_span) {
		m_inner_span->End();
	}
}

bool OpenTelemetryTrace::start_segment(const FString &n_name) {

	if (m_scopes.Find(n_name)) {
		return false;
	}

	InnerSegment new_seg{ *m_tracer, n_name };
	m_scopes.Emplace(n_name, MoveTemp(new_seg));

	return true;
}

bool OpenTelemetryTrace::end_segment(const FString &n_name, const bool n_error /* = false*/) noexcept {
	
	// OpenTelemetry doesn't seem to have a notion of error flag here
	return m_scopes.Remove(n_name) == 1;
}


#endif // WITH_GOOGLECLOUD_SDK
