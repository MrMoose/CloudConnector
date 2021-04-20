/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "BlindTracingImpl.h"
#include "ICloudConnector.h"

void BlindTracingImpl::write_trace_document(CloudTrace &n_trace) {

	UE_LOG(LogCloudConnector, Display, TEXT("Ignore trace document (%s)."), *n_trace.m_payload->m_trace_id);
}
