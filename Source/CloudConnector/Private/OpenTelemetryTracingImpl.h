/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

 // I have not found a way to exclude those files from the build if Google Cloud is 
 // not required. So I fake it blind this sway
#ifdef WITH_GOOGLECLOUD_SDK

#include "CoreMinimal.h"
#include "ICloudTracing.h"

#pragma warning( push )
#pragma warning( disable : 4668 )

#include <opentelemetry/trace/provider.h>

#pragma warning( pop )


/**
 * @brief OpenTelemetry Tracing implementation
 * Highly experimental. Work in progress. Do not use!
 * Your computer will explode.
 */
class OpenTelemetryTracingImpl : public ICloudTracing {

	public:
		OpenTelemetryTracingImpl();
		ICloudTracePtr start_trace(const FString &n_trace_id) override;

	private:
		void finish_trace(ICloudTrace *n_trace) override;

		opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> m_tracer;
};

// I have not found a way to exclude those files from the build if Google Cloud is 
// not required. So I fake it blind this sway
#else // WITH_GOOGLECLOUD_SDK
#include "CoreMinimal.h"
#include "BlindTracingImpl.h"
using OpenTelemetryTracingImpl = BlindTracingImpl;
#endif // WITH_GOOGLECLOUD_SDK
