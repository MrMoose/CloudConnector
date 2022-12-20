/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudTracing.h"

/** ICloudTracing implementation using AWS XRay
 */
class AWSTracingImpl : public ICloudTracing {

	public:
		ICloudTracePtr start_trace(const FString &n_trace_id) override;
		void finish_trace(ICloudTrace *n_trace) override;
		
	protected:
		void write_trace_document(class AWSTrace &n_trace);
		TSharedPtr<class FJsonObject> create_trace_document(const TracePayload &n_trace) const;
};
