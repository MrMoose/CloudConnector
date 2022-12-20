/* (c) 2022 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#ifdef WITH_GOOGLECLOUD_SDK

#include "OpenTelemetryTracingImpl.h"
#include "ICloudConnector.h"
#include "OpenTelemetryTrace.h"

#pragma warning( push )
#pragma warning( disable : 4668 )

#include <opentelemetry/trace/tracer.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/tracer_provider.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_http_exporter.h>
#include <opentelemetry/exporters/otlp/otlp_http_exporter_factory.h>
#include <opentelemetry/sdk/version/version.h>
#include <opentelemetry/sdk/trace/exporter.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/trace/provider.h"

#pragma warning( pop )

#include <string>


namespace trace = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace trace_otlp_exp = opentelemetry::exporter::otlp;
namespace nostd = opentelemetry::nostd;

OpenTelemetryTracingImpl::OpenTelemetryTracingImpl() {
	
	std::unique_ptr<trace_sdk::SpanExporter> exporter = [] {

		// I will default to the gRPC exporter unless the user sets an http endpoint
		const FString http_endpoint_url = readenv(TEXT("CLOUDCONNECTOR_OTLP_HTTP_ENDPPOINT"));
		if (!http_endpoint_url.IsEmpty()) {
			trace_otlp_exp::OtlpHttpExporterOptions opts;
			opts.console_debug = true;
			opts.url = TCHAR_TO_ANSI(*http_endpoint_url);
			return trace_otlp_exp::OtlpHttpExporterFactory::Create(opts);
		}

		const FString grpc_endpoint_url = readenv(TEXT("CLOUDCONNECTOR_OTLP_GRPC_ENDPPOINT"));
		if (!grpc_endpoint_url.IsEmpty()) {
			trace_otlp_exp::OtlpGrpcExporterOptions opts;
			opts.endpoint = TCHAR_TO_ANSI(*grpc_endpoint_url);
			return trace_otlp_exp::OtlpGrpcExporterFactory::Create(opts);
		} else {
			return trace_otlp_exp::OtlpGrpcExporterFactory::Create();
		}
	}();

	std::unique_ptr<trace_sdk::SpanProcessor> processor = 
			trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));

	std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
			trace_sdk::TracerProviderFactory::Create(std::move(processor));

	// Set the global trace provider
	trace::Provider::SetTracerProvider(provider);
}

ICloudTracePtr OpenTelemetryTracingImpl::start_trace(const FString &n_trace_id) {

	if (n_trace_id.IsEmpty()) {
		return {};
	}

	if (!m_tracer) {
		const nostd::shared_ptr<trace::TracerProvider> provider = trace::Provider::GetTracerProvider();
		checkf(provider, TEXT("Must not be null according to specs"));
		m_tracer = provider->GetTracer("CloudConnector", OPENTELEMETRY_SDK_VERSION);
	}

	std::string given_id = TCHAR_TO_ANSI(*n_trace_id);

	int current_byte = 0;
	uint8 byte_id[trace::TraceId::kSize] = { 0 };

	for (std::string::size_type i = 0; i < given_id.size(); i += 2) {
		const std::string byte_str = given_id.substr(i, 2);
		byte_id[current_byte++] = static_cast<uint8>(stoi(byte_str, nullptr, 16));
	}

	trace::TraceId trace_id{ nostd::span<const uint8_t, trace::TraceId::kSize>{ byte_id } };

	trace::StartSpanOptions opts;
	opts.parent = trace::SpanContext{ trace_id, trace::SpanId{}, trace::TraceFlags{}, true };

	ICloudTracePtr new_trace = MakeShared<OpenTelemetryTrace, ESPMode::ThreadSafe>(n_trace_id, m_tracer, MoveTemp(opts));
	return new_trace;
}

void OpenTelemetryTracingImpl::finish_trace(ICloudTrace *n_trace) {

}

#endif // WITH_GOOGLECLOUD_SDK
