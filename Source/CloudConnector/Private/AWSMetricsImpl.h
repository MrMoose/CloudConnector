/* (c) 2022 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudMetrics.h"

/**
 * ICloudMetrics implementation using AWS CloudWatch
 */
class AWSMetricsImpl : public ICloudMetrics {

	public:
		AWSMetricsImpl(const class ACloudConnector *n_config);
};
