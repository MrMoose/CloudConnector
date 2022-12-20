/* (c) 2022 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#ifdef WITH_GOOGLECLOUD_SDK

#include "CoreMinimal.h"
#include "ICloudMetrics.h"

#pragma warning( push )
#pragma warning( disable : 4668 )

#include <opentelemetry/metrics/meter_provider.h>

#pragma warning( pop )

#include <memory>


/**
 * ICloudMetrics implementation using OpenTelemetry for Google Cloud.
 * This doesn't do anything yet
 */
class OpenTelemetryMetricsImpl : public ICloudStorage {

	public:
		OpenTelemetryMetricsImpl();

		void increment_named_counter(const FString n_name);

	private:

		std::unique_ptr<opentelemetry::metrics::MeterProvider> m_meter_provider;

};

// I have not found a way to exclude those files from the build if Google Cloud is 
// not required. So I fake it blind this way
#else // WITH_GOOGLECLOUD_SDK
#include "CoreMinimal.h"
#include "BlindMetricsImpl.h"
using OpenTelemetryMetricsImpl = BlindMetricsImpl;
#endif // WITH_GOOGLECLOUD_SDK
