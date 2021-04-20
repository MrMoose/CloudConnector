/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "AWSTracingImpl.h"
#include "ICloudConnector.h"
#include "Utilities.h"

#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"
#include "Json/Public/Policies/CondensedJsonPrintPolicy.h"
#include "Async/Async.h"

 // AWS SDK
#include "Windows/PreWindowsApi.h"
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/client/AWSError.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>
#include <aws/xray/XRayClient.h>
#include <aws/xray/model/PutTraceSegmentsRequest.h>
#include <aws/xray/model/PutTraceSegmentsResult.h>
#include "Windows/PostWindowsApi.h"

// std
#include <string>

/* Those objects are supposedly threadsafe and tests have shown that they really appear to be so
 * it seems to be safe to concurrently access this from multiple threads at once
 */
static Aws::UniquePtr<Aws::XRay::XRayClient> s_xray_client;

void AWSTracingImpl::write_trace_document(CloudTrace &n_trace) {

	checkf(n_trace.m_payload, TEXT("Broken trace object. What happened to you?"));

	// We go async right away to not block the game thread for this.
	// Since this is called while the trace itself is getting destroyed, I will steal its payload
	// to keep it around in our handler while it does its job.
	// The trace is ours now.
	Async(EAsyncExecution::ThreadPool, [this, payload{ MoveTemp(n_trace.m_payload) }]{
		
		using namespace Aws::XRay::Model;

		// First check for the global client object and create it if needed
		if (!s_xray_client) {
			// Using a pool executor is supposed to render the client thread safe
			Aws::Client::ClientConfiguration client_config;
			client_config.enableEndpointDiscovery = use_endpoint_discovery();
			const FString xray_endpoint = readenv(TEXT("CLOUDCONNECTOR_XRAY_ENDPOINT"));
			if (!xray_endpoint.IsEmpty()) {
				client_config.endpointOverride = TCHAR_TO_UTF8(*xray_endpoint);
			}

			s_xray_client = Aws::MakeUnique<Aws::XRay::XRayClient>("CCXRayAllocation", client_config);
		}

		// Now create an XRay trace document from our trace
		TSharedPtr<FJsonObject> trace_document = create_trace_document(*payload);
		checkf(trace_document, TEXT("Failed to create trace document"));

		// Create a raw json string. Apparently it must be unformatted (condensed)
		// as otherwise XRay didn't accept it. Weird that is.
		FString json;
		TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&json);
		FJsonSerializer::Serialize(trace_document.ToSharedRef(), writer);

		// And send it to XRay
		PutTraceSegmentsRequest request;
		request.AddTraceSegmentDocuments(TCHAR_TO_UTF8(*json));

		PutTraceSegmentsOutcome oc = s_xray_client->PutTraceSegments(request);
		if (oc.IsSuccess()) 		{
			if (oc.GetResult().GetUnprocessedTraceSegments().size()) {
				UE_LOG(LogCloudConnector, Warning, TEXT("Unprocessed segments while sending X-Ray document."));
			}
		} else {
			// This never seems to happen. It's a quick UDP send. Normally the error will be reported by unprocessed segments
			UE_LOG(LogCloudConnector, Warning, TEXT("Failed to send X-Ray document: %s"), UTF8_TO_TCHAR(oc.GetError().GetMessage().c_str()));
		}
	});
}

TSharedPtr<FJsonObject> AWSTracingImpl::create_trace_document(const TracePayload &n_trace) const {

	using DocumentPtr = TSharedPtr<FJsonObject>;

	DocumentPtr document = MakeShared<FJsonObject>();

	// See here for scheme
	// https://docs.aws.amazon.com/xray/latest/devguide/xray-api-segmentdocuments.html
	document->SetStringField(TEXT("name"), TEXT("CloudConnector"));
	document->SetStringField(TEXT("trace_id"), n_trace.m_trace_id);
	document->SetStringField(TEXT("origin"), TEXT("AWS::EC2::Instance"));

	// Each document needs to contain a segment ID.
	// I am assuming a random id is needed but I don't know
	document->SetStringField(TEXT("id"), random_aws_trace_id());

	// begin and end, seconds since epoch in milli precision
	document->SetNumberField(TEXT("start_time"), n_trace.m_start);
	document->SetNumberField(TEXT("end_time"), n_trace.m_end);

	// Gather all recorded subsegments and put them in
	bool any_segment_had_an_error = false;
	TArray<TSharedPtr<FJsonValue> > subsegment_array;
	
	for (const TracePayload::Segment &s : n_trace.m_segments) {
		DocumentPtr segment_doc = MakeShared<FJsonObject>();
		segment_doc->SetNumberField(TEXT("start_time"), s.m_start);
		segment_doc->SetNumberField(TEXT("end_time"), s.m_end);
		segment_doc->SetStringField(TEXT("name"), s.m_name);
		segment_doc->SetStringField(TEXT("namespace"), TEXT("remote"));
		segment_doc->SetStringField(TEXT("id"), random_aws_trace_id());
		segment_doc->SetBoolField(TEXT("in_progress"), false);
		if (s.m_error) {
			segment_doc->SetBoolField(TEXT("fault"), true);
			any_segment_had_an_error = true;
		}

		subsegment_array.Add(MakeShareable(new FJsonValueObject(segment_doc)));
	}
	
	if (subsegment_array.Num()) {
		document->SetArrayField("subsegments", subsegment_array);
	}

	document->SetBoolField(TEXT("in_progress"), false);

	if (any_segment_had_an_error) {
		document->SetBoolField(TEXT("fault"), true);
	}

	return document;
}
