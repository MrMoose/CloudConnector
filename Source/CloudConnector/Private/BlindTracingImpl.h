/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudTracing.h"

/** ICloudTracing implementation doing nothing. Supposed to be used for testing later
 */
class BlindTracingImpl : public ICloudTracing {

	protected:
		void write_trace_document(CloudTrace &n_trace) override;
};
