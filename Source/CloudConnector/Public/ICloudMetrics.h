/* (c) 2022 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudTracing.h"
#include "Containers/ArrayView.h"

//#include "ICloudMetrics.generated.h"

/** 
 * Implementation wrapper for various metrics collectors
 * For GCP this is implemented as OpenTelemetry. For AWS it will be CloudWatch 
 */
class CLOUDCONNECTOR_API ICloudMetrics {

	public:
		virtual ~ICloudMetrics() noexcept = default;

};

