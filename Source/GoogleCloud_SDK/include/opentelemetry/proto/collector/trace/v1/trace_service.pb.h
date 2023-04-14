// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: opentelemetry/proto/collector/trace/v1/trace_service.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "opentelemetry/proto/trace/v1/trace.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto;
namespace opentelemetry {
namespace proto {
namespace collector {
namespace trace {
namespace v1 {
class ExportTracePartialSuccess;
struct ExportTracePartialSuccessDefaultTypeInternal;
extern ExportTracePartialSuccessDefaultTypeInternal _ExportTracePartialSuccess_default_instance_;
class ExportTraceServiceRequest;
struct ExportTraceServiceRequestDefaultTypeInternal;
extern ExportTraceServiceRequestDefaultTypeInternal _ExportTraceServiceRequest_default_instance_;
class ExportTraceServiceResponse;
struct ExportTraceServiceResponseDefaultTypeInternal;
extern ExportTraceServiceResponseDefaultTypeInternal _ExportTraceServiceResponse_default_instance_;
}  // namespace v1
}  // namespace trace
}  // namespace collector
}  // namespace proto
}  // namespace opentelemetry
PROTOBUF_NAMESPACE_OPEN
template<> ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* Arena::CreateMaybeMessage<::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess>(Arena*);
template<> ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest* Arena::CreateMaybeMessage<::opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest>(Arena*);
template<> ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceResponse* Arena::CreateMaybeMessage<::opentelemetry::proto::collector::trace::v1::ExportTraceServiceResponse>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace opentelemetry {
namespace proto {
namespace collector {
namespace trace {
namespace v1 {

// ===================================================================

class ExportTraceServiceRequest final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:opentelemetry.proto.collector.trace.v1.ExportTraceServiceRequest) */ {
 public:
  inline ExportTraceServiceRequest() : ExportTraceServiceRequest(nullptr) {}
  ~ExportTraceServiceRequest() override;
  explicit PROTOBUF_CONSTEXPR ExportTraceServiceRequest(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  ExportTraceServiceRequest(const ExportTraceServiceRequest& from);
  ExportTraceServiceRequest(ExportTraceServiceRequest&& from) noexcept
    : ExportTraceServiceRequest() {
    *this = ::std::move(from);
  }

  inline ExportTraceServiceRequest& operator=(const ExportTraceServiceRequest& from) {
    CopyFrom(from);
    return *this;
  }
  inline ExportTraceServiceRequest& operator=(ExportTraceServiceRequest&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const ExportTraceServiceRequest& default_instance() {
    return *internal_default_instance();
  }
  static inline const ExportTraceServiceRequest* internal_default_instance() {
    return reinterpret_cast<const ExportTraceServiceRequest*>(
               &_ExportTraceServiceRequest_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(ExportTraceServiceRequest& a, ExportTraceServiceRequest& b) {
    a.Swap(&b);
  }
  inline void Swap(ExportTraceServiceRequest* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(ExportTraceServiceRequest* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  ExportTraceServiceRequest* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<ExportTraceServiceRequest>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const ExportTraceServiceRequest& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const ExportTraceServiceRequest& from) {
    ExportTraceServiceRequest::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ExportTraceServiceRequest* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "opentelemetry.proto.collector.trace.v1.ExportTraceServiceRequest";
  }
  protected:
  explicit ExportTraceServiceRequest(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kResourceSpansFieldNumber = 1,
  };
  // repeated .opentelemetry.proto.trace.v1.ResourceSpans resource_spans = 1;
  int resource_spans_size() const;
  private:
  int _internal_resource_spans_size() const;
  public:
  void clear_resource_spans();
  ::opentelemetry::proto::trace::v1::ResourceSpans* mutable_resource_spans(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::opentelemetry::proto::trace::v1::ResourceSpans >*
      mutable_resource_spans();
  private:
  const ::opentelemetry::proto::trace::v1::ResourceSpans& _internal_resource_spans(int index) const;
  ::opentelemetry::proto::trace::v1::ResourceSpans* _internal_add_resource_spans();
  public:
  const ::opentelemetry::proto::trace::v1::ResourceSpans& resource_spans(int index) const;
  ::opentelemetry::proto::trace::v1::ResourceSpans* add_resource_spans();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::opentelemetry::proto::trace::v1::ResourceSpans >&
      resource_spans() const;

  // @@protoc_insertion_point(class_scope:opentelemetry.proto.collector.trace.v1.ExportTraceServiceRequest)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::opentelemetry::proto::trace::v1::ResourceSpans > resource_spans_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto;
};
// -------------------------------------------------------------------

class ExportTraceServiceResponse final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:opentelemetry.proto.collector.trace.v1.ExportTraceServiceResponse) */ {
 public:
  inline ExportTraceServiceResponse() : ExportTraceServiceResponse(nullptr) {}
  ~ExportTraceServiceResponse() override;
  explicit PROTOBUF_CONSTEXPR ExportTraceServiceResponse(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  ExportTraceServiceResponse(const ExportTraceServiceResponse& from);
  ExportTraceServiceResponse(ExportTraceServiceResponse&& from) noexcept
    : ExportTraceServiceResponse() {
    *this = ::std::move(from);
  }

  inline ExportTraceServiceResponse& operator=(const ExportTraceServiceResponse& from) {
    CopyFrom(from);
    return *this;
  }
  inline ExportTraceServiceResponse& operator=(ExportTraceServiceResponse&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const ExportTraceServiceResponse& default_instance() {
    return *internal_default_instance();
  }
  static inline const ExportTraceServiceResponse* internal_default_instance() {
    return reinterpret_cast<const ExportTraceServiceResponse*>(
               &_ExportTraceServiceResponse_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(ExportTraceServiceResponse& a, ExportTraceServiceResponse& b) {
    a.Swap(&b);
  }
  inline void Swap(ExportTraceServiceResponse* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(ExportTraceServiceResponse* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  ExportTraceServiceResponse* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<ExportTraceServiceResponse>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const ExportTraceServiceResponse& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const ExportTraceServiceResponse& from) {
    ExportTraceServiceResponse::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ExportTraceServiceResponse* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "opentelemetry.proto.collector.trace.v1.ExportTraceServiceResponse";
  }
  protected:
  explicit ExportTraceServiceResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kPartialSuccessFieldNumber = 1,
  };
  // .opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess partial_success = 1;
  bool has_partial_success() const;
  private:
  bool _internal_has_partial_success() const;
  public:
  void clear_partial_success();
  const ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess& partial_success() const;
  PROTOBUF_NODISCARD ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* release_partial_success();
  ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* mutable_partial_success();
  void set_allocated_partial_success(::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* partial_success);
  private:
  const ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess& _internal_partial_success() const;
  ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* _internal_mutable_partial_success();
  public:
  void unsafe_arena_set_allocated_partial_success(
      ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* partial_success);
  ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* unsafe_arena_release_partial_success();

  // @@protoc_insertion_point(class_scope:opentelemetry.proto.collector.trace.v1.ExportTraceServiceResponse)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* partial_success_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto;
};
// -------------------------------------------------------------------

class ExportTracePartialSuccess final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess) */ {
 public:
  inline ExportTracePartialSuccess() : ExportTracePartialSuccess(nullptr) {}
  ~ExportTracePartialSuccess() override;
  explicit PROTOBUF_CONSTEXPR ExportTracePartialSuccess(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  ExportTracePartialSuccess(const ExportTracePartialSuccess& from);
  ExportTracePartialSuccess(ExportTracePartialSuccess&& from) noexcept
    : ExportTracePartialSuccess() {
    *this = ::std::move(from);
  }

  inline ExportTracePartialSuccess& operator=(const ExportTracePartialSuccess& from) {
    CopyFrom(from);
    return *this;
  }
  inline ExportTracePartialSuccess& operator=(ExportTracePartialSuccess&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const ExportTracePartialSuccess& default_instance() {
    return *internal_default_instance();
  }
  static inline const ExportTracePartialSuccess* internal_default_instance() {
    return reinterpret_cast<const ExportTracePartialSuccess*>(
               &_ExportTracePartialSuccess_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(ExportTracePartialSuccess& a, ExportTracePartialSuccess& b) {
    a.Swap(&b);
  }
  inline void Swap(ExportTracePartialSuccess* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(ExportTracePartialSuccess* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  ExportTracePartialSuccess* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<ExportTracePartialSuccess>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const ExportTracePartialSuccess& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const ExportTracePartialSuccess& from) {
    ExportTracePartialSuccess::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ExportTracePartialSuccess* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess";
  }
  protected:
  explicit ExportTracePartialSuccess(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kErrorMessageFieldNumber = 2,
    kRejectedSpansFieldNumber = 1,
  };
  // string error_message = 2;
  void clear_error_message();
  const std::string& error_message() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_error_message(ArgT0&& arg0, ArgT... args);
  std::string* mutable_error_message();
  PROTOBUF_NODISCARD std::string* release_error_message();
  void set_allocated_error_message(std::string* error_message);
  private:
  const std::string& _internal_error_message() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_error_message(const std::string& value);
  std::string* _internal_mutable_error_message();
  public:

  // int64 rejected_spans = 1;
  void clear_rejected_spans();
  int64_t rejected_spans() const;
  void set_rejected_spans(int64_t value);
  private:
  int64_t _internal_rejected_spans() const;
  void _internal_set_rejected_spans(int64_t value);
  public:

  // @@protoc_insertion_point(class_scope:opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr error_message_;
    int64_t rejected_spans_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// ExportTraceServiceRequest

// repeated .opentelemetry.proto.trace.v1.ResourceSpans resource_spans = 1;
inline int ExportTraceServiceRequest::_internal_resource_spans_size() const {
  return _impl_.resource_spans_.size();
}
inline int ExportTraceServiceRequest::resource_spans_size() const {
  return _internal_resource_spans_size();
}
inline ::opentelemetry::proto::trace::v1::ResourceSpans* ExportTraceServiceRequest::mutable_resource_spans(int index) {
  // @@protoc_insertion_point(field_mutable:opentelemetry.proto.collector.trace.v1.ExportTraceServiceRequest.resource_spans)
  return _impl_.resource_spans_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::opentelemetry::proto::trace::v1::ResourceSpans >*
ExportTraceServiceRequest::mutable_resource_spans() {
  // @@protoc_insertion_point(field_mutable_list:opentelemetry.proto.collector.trace.v1.ExportTraceServiceRequest.resource_spans)
  return &_impl_.resource_spans_;
}
inline const ::opentelemetry::proto::trace::v1::ResourceSpans& ExportTraceServiceRequest::_internal_resource_spans(int index) const {
  return _impl_.resource_spans_.Get(index);
}
inline const ::opentelemetry::proto::trace::v1::ResourceSpans& ExportTraceServiceRequest::resource_spans(int index) const {
  // @@protoc_insertion_point(field_get:opentelemetry.proto.collector.trace.v1.ExportTraceServiceRequest.resource_spans)
  return _internal_resource_spans(index);
}
inline ::opentelemetry::proto::trace::v1::ResourceSpans* ExportTraceServiceRequest::_internal_add_resource_spans() {
  return _impl_.resource_spans_.Add();
}
inline ::opentelemetry::proto::trace::v1::ResourceSpans* ExportTraceServiceRequest::add_resource_spans() {
  ::opentelemetry::proto::trace::v1::ResourceSpans* _add = _internal_add_resource_spans();
  // @@protoc_insertion_point(field_add:opentelemetry.proto.collector.trace.v1.ExportTraceServiceRequest.resource_spans)
  return _add;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::opentelemetry::proto::trace::v1::ResourceSpans >&
ExportTraceServiceRequest::resource_spans() const {
  // @@protoc_insertion_point(field_list:opentelemetry.proto.collector.trace.v1.ExportTraceServiceRequest.resource_spans)
  return _impl_.resource_spans_;
}

// -------------------------------------------------------------------

// ExportTraceServiceResponse

// .opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess partial_success = 1;
inline bool ExportTraceServiceResponse::_internal_has_partial_success() const {
  return this != internal_default_instance() && _impl_.partial_success_ != nullptr;
}
inline bool ExportTraceServiceResponse::has_partial_success() const {
  return _internal_has_partial_success();
}
inline void ExportTraceServiceResponse::clear_partial_success() {
  if (GetArenaForAllocation() == nullptr && _impl_.partial_success_ != nullptr) {
    delete _impl_.partial_success_;
  }
  _impl_.partial_success_ = nullptr;
}
inline const ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess& ExportTraceServiceResponse::_internal_partial_success() const {
  const ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* p = _impl_.partial_success_;
  return p != nullptr ? *p : reinterpret_cast<const ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess&>(
      ::opentelemetry::proto::collector::trace::v1::_ExportTracePartialSuccess_default_instance_);
}
inline const ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess& ExportTraceServiceResponse::partial_success() const {
  // @@protoc_insertion_point(field_get:opentelemetry.proto.collector.trace.v1.ExportTraceServiceResponse.partial_success)
  return _internal_partial_success();
}
inline void ExportTraceServiceResponse::unsafe_arena_set_allocated_partial_success(
    ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* partial_success) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.partial_success_);
  }
  _impl_.partial_success_ = partial_success;
  if (partial_success) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:opentelemetry.proto.collector.trace.v1.ExportTraceServiceResponse.partial_success)
}
inline ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* ExportTraceServiceResponse::release_partial_success() {
  
  ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* temp = _impl_.partial_success_;
  _impl_.partial_success_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* ExportTraceServiceResponse::unsafe_arena_release_partial_success() {
  // @@protoc_insertion_point(field_release:opentelemetry.proto.collector.trace.v1.ExportTraceServiceResponse.partial_success)
  
  ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* temp = _impl_.partial_success_;
  _impl_.partial_success_ = nullptr;
  return temp;
}
inline ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* ExportTraceServiceResponse::_internal_mutable_partial_success() {
  
  if (_impl_.partial_success_ == nullptr) {
    auto* p = CreateMaybeMessage<::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess>(GetArenaForAllocation());
    _impl_.partial_success_ = p;
  }
  return _impl_.partial_success_;
}
inline ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* ExportTraceServiceResponse::mutable_partial_success() {
  ::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* _msg = _internal_mutable_partial_success();
  // @@protoc_insertion_point(field_mutable:opentelemetry.proto.collector.trace.v1.ExportTraceServiceResponse.partial_success)
  return _msg;
}
inline void ExportTraceServiceResponse::set_allocated_partial_success(::opentelemetry::proto::collector::trace::v1::ExportTracePartialSuccess* partial_success) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete _impl_.partial_success_;
  }
  if (partial_success) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(partial_success);
    if (message_arena != submessage_arena) {
      partial_success = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, partial_success, submessage_arena);
    }
    
  } else {
    
  }
  _impl_.partial_success_ = partial_success;
  // @@protoc_insertion_point(field_set_allocated:opentelemetry.proto.collector.trace.v1.ExportTraceServiceResponse.partial_success)
}

// -------------------------------------------------------------------

// ExportTracePartialSuccess

// int64 rejected_spans = 1;
inline void ExportTracePartialSuccess::clear_rejected_spans() {
  _impl_.rejected_spans_ = int64_t{0};
}
inline int64_t ExportTracePartialSuccess::_internal_rejected_spans() const {
  return _impl_.rejected_spans_;
}
inline int64_t ExportTracePartialSuccess::rejected_spans() const {
  // @@protoc_insertion_point(field_get:opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess.rejected_spans)
  return _internal_rejected_spans();
}
inline void ExportTracePartialSuccess::_internal_set_rejected_spans(int64_t value) {
  
  _impl_.rejected_spans_ = value;
}
inline void ExportTracePartialSuccess::set_rejected_spans(int64_t value) {
  _internal_set_rejected_spans(value);
  // @@protoc_insertion_point(field_set:opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess.rejected_spans)
}

// string error_message = 2;
inline void ExportTracePartialSuccess::clear_error_message() {
  _impl_.error_message_.ClearToEmpty();
}
inline const std::string& ExportTracePartialSuccess::error_message() const {
  // @@protoc_insertion_point(field_get:opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess.error_message)
  return _internal_error_message();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void ExportTracePartialSuccess::set_error_message(ArgT0&& arg0, ArgT... args) {
 
 _impl_.error_message_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess.error_message)
}
inline std::string* ExportTracePartialSuccess::mutable_error_message() {
  std::string* _s = _internal_mutable_error_message();
  // @@protoc_insertion_point(field_mutable:opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess.error_message)
  return _s;
}
inline const std::string& ExportTracePartialSuccess::_internal_error_message() const {
  return _impl_.error_message_.Get();
}
inline void ExportTracePartialSuccess::_internal_set_error_message(const std::string& value) {
  
  _impl_.error_message_.Set(value, GetArenaForAllocation());
}
inline std::string* ExportTracePartialSuccess::_internal_mutable_error_message() {
  
  return _impl_.error_message_.Mutable(GetArenaForAllocation());
}
inline std::string* ExportTracePartialSuccess::release_error_message() {
  // @@protoc_insertion_point(field_release:opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess.error_message)
  return _impl_.error_message_.Release();
}
inline void ExportTracePartialSuccess::set_allocated_error_message(std::string* error_message) {
  if (error_message != nullptr) {
    
  } else {
    
  }
  _impl_.error_message_.SetAllocated(error_message, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.error_message_.IsDefault()) {
    _impl_.error_message_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:opentelemetry.proto.collector.trace.v1.ExportTracePartialSuccess.error_message)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace trace
}  // namespace collector
}  // namespace proto
}  // namespace opentelemetry

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_opentelemetry_2fproto_2fcollector_2ftrace_2fv1_2ftrace_5fservice_2eproto