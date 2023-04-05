/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#ifdef WITH_GOOGLECLOUD_SDK

#include "CoreMinimal.h"
#include "ICloudTracing.h"

#pragma warning( push )
#pragma warning( disable : 4668 )

#include <opentelemetry/trace/provider.h>

#pragma warning( pop )

/**
 * @brief A trace implementation sending its results out via lib OpenTelemetry.
 * Highly experimental and work in progress. Do not use!
 */
class OpenTelemetryTrace final : public ICloudTrace {

	public:
		OpenTelemetryTrace() = delete;
		OpenTelemetryTrace(const OpenTelemetryTrace &) = delete;
		~OpenTelemetryTrace() noexcept;

		ScopedSegment scoped_segment(const FString &n_name) override;
		bool start_segment(const FString &n_name) override;
		bool end_segment(const FString &n_name, const bool n_error = false) noexcept override;
		const FString &id() const noexcept override;

	private:
		// only ICloudTracing may create such objects
		template <typename ObjectType, ESPMode Mode>
		friend class SharedPointerInternals::TIntrusiveReferenceController;
		friend class ICloudTracing;
		friend class OpenTelemetryTracingImpl;
		
		OpenTelemetryTrace(const FString &n_trace_id,
			opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> n_tracer, 
			opentelemetry::trace::StartSpanOptions &&n_options);
		
		const FString                                                  m_id;
		opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> m_tracer;
		opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>   m_outer_span;
		opentelemetry::trace::Scope                                    m_outer_scope;

		struct InnerSegment {
			InnerSegment(opentelemetry::trace::Tracer &n_tracer, const FString &n_name);
			InnerSegment(InnerSegment &&) = default;
			InnerSegment(const InnerSegment &) = delete;
			~InnerSegment();
			opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>  m_inner_span;
			opentelemetry::trace::Scope                                   m_inner_scope;
		};

		TMap<FString, InnerSegment>    m_scopes;
};

// I have not found a way to exclude those files from the build if Google Cloud is 
// not required. So I fake it blind this way
#else // WITH_GOOGLECLOUD_SDK
#include "CoreMinimal.h"
#include "BlindTrace.h"
using OpenTelemetryTrace = BlindTrace;
#endif // WITH_GOOGLECLOUD_SDK
