/* (c) 2022 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */

#ifdef WITH_GOOGLECLOUD_SDK

#include "OpenTelemetryMetricsImpl.h"
#include "ICloudConnector.h"
#include "TraceMacros.h"

#include "Async/Async.h"
#include "Internationalization/Regex.h"

#pragma warning( push )
#pragma warning( disable : 4668 )

#include <opentelemetry/metrics/meter.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/sdk/metrics/meter.h>
#include <opentelemetry/sdk/metrics/meter_provider.h>

#pragma warning( pop )

namespace metrics_api = opentelemetry::metrics;
namespace metric_sdk  = opentelemetry::sdk::metrics;


OpenTelemetryMetricsImpl::OpenTelemetryMetricsImpl()
		: m_meter_provider{ std::make_unique<metric_sdk::MeterProvider>() } {

}


void OpenTelemetryMetricsImpl::increment_named_counter(const FString n_name) {

	auto meter = m_meter_provider->GetMeter("MyMeter");

	auto counter = meter->CreateUInt64Counter("MyCounter", "uint64 counter as in example", "ticks");

	counter->Add(1);
}

#endif // WITH_GOOGLECLOUD_SDK
