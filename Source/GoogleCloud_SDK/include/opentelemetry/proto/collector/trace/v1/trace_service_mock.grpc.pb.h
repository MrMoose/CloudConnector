// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: opentelemetry/proto/collector/trace/v1/trace_service.proto

#ifndef GRPC_MOCK_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto__INCLUDED
#define GRPC_MOCK_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto__INCLUDED

#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"

#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/sync_stream.h>
#include <gmock/gmock.h>
namespace opentelemetry {
namespace proto {
namespace collector {
namespace trace {
namespace v1 {

class MockTraceServiceStub : public TraceService::StubInterface {
 public:
  MOCK_METHOD3(Export, ::grpc::Status(::grpc::ClientContext* context, const ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest& request, ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceResponse* response));
  MOCK_METHOD3(AsyncExportRaw, ::grpc::ClientAsyncResponseReaderInterface< ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceResponse>*(::grpc::ClientContext* context, const ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncExportRaw, ::grpc::ClientAsyncResponseReaderInterface< ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceResponse>*(::grpc::ClientContext* context, const ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest& request, ::grpc::CompletionQueue* cq));
};

}  // namespace v1
}  // namespace trace
}  // namespace collector
}  // namespace proto
}  // namespace opentelemetry


#endif  // GRPC_MOCK_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto__INCLUDED