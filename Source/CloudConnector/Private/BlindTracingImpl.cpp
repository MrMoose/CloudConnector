/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "BlindTracingImpl.h"
#include "ICloudConnector.h"
#include "BlindTrace.h"


ICloudTracePtr BlindTracingImpl::start_trace(const FString &n_trace_id) {
	
	return MakeShared<BlindTrace, ESPMode::ThreadSafe>(n_trace_id);
}

void BlindTracingImpl::finish_trace(ICloudTrace *n_trace) {

	if (n_trace) {
		UE_LOG(LogCloudConnector, Display, TEXT("Ignore finish trace of (%s)."), *n_trace->id());
	} else {
		UE_LOG(LogCloudConnector, Display, TEXT("Ignore finish null trace"));
	}
}
