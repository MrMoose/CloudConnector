/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudTracing.h"

/** ICloudTracing implementation eventually using Google Tracing
 *  This is a placeholder implementation, which shall eventually use Cloud Trace
 *  as a backend. As it doesn't look like this is being worked at by the 
 *  Google Cloud SDK I am afraid this will take a quite some time.
 * 
 *  I am considering to write the data somehow pretty printed to logs instead
 *  but couldn't bothered by now.
 */
class GoogleTracingImpl : public ICloudTracing {

	protected:
		void write_trace_document(CloudTrace &n_trace) override;
};
