// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/spanner/admin/instance/v1/common.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_google_2fspanner_2fadmin_2finstance_2fv1_2fcommon_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_google_2fspanner_2fadmin_2finstance_2fv1_2fcommon_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021006 < PROTOBUF_MIN_PROTOC_VERSION
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
#include <google/protobuf/timestamp.pb.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_google_2fspanner_2fadmin_2finstance_2fv1_2fcommon_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_google_2fspanner_2fadmin_2finstance_2fv1_2fcommon_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_google_2fspanner_2fadmin_2finstance_2fv1_2fcommon_2eproto;
namespace google {
namespace spanner {
namespace admin {
namespace instance {
namespace v1 {
class OperationProgress;
struct OperationProgressDefaultTypeInternal;
extern OperationProgressDefaultTypeInternal _OperationProgress_default_instance_;
}  // namespace v1
}  // namespace instance
}  // namespace admin
}  // namespace spanner
}  // namespace google
PROTOBUF_NAMESPACE_OPEN
template<> ::google::spanner::admin::instance::v1::OperationProgress* Arena::CreateMaybeMessage<::google::spanner::admin::instance::v1::OperationProgress>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace google {
namespace spanner {
namespace admin {
namespace instance {
namespace v1 {

// ===================================================================

class OperationProgress final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:google.spanner.admin.instance.v1.OperationProgress) */ {
 public:
  inline OperationProgress() : OperationProgress(nullptr) {}
  ~OperationProgress() override;
  explicit PROTOBUF_CONSTEXPR OperationProgress(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  OperationProgress(const OperationProgress& from);
  OperationProgress(OperationProgress&& from) noexcept
    : OperationProgress() {
    *this = ::std::move(from);
  }

  inline OperationProgress& operator=(const OperationProgress& from) {
    CopyFrom(from);
    return *this;
  }
  inline OperationProgress& operator=(OperationProgress&& from) noexcept {
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
  static const OperationProgress& default_instance() {
    return *internal_default_instance();
  }
  static inline const OperationProgress* internal_default_instance() {
    return reinterpret_cast<const OperationProgress*>(
               &_OperationProgress_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(OperationProgress& a, OperationProgress& b) {
    a.Swap(&b);
  }
  inline void Swap(OperationProgress* other) {
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
  void UnsafeArenaSwap(OperationProgress* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  OperationProgress* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<OperationProgress>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const OperationProgress& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const OperationProgress& from) {
    OperationProgress::MergeImpl(*this, from);
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
  void InternalSwap(OperationProgress* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "google.spanner.admin.instance.v1.OperationProgress";
  }
  protected:
  explicit OperationProgress(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kStartTimeFieldNumber = 2,
    kEndTimeFieldNumber = 3,
    kProgressPercentFieldNumber = 1,
  };
  // .google.protobuf.Timestamp start_time = 2;
  bool has_start_time() const;
  private:
  bool _internal_has_start_time() const;
  public:
  void clear_start_time();
  const ::PROTOBUF_NAMESPACE_ID::Timestamp& start_time() const;
  PROTOBUF_NODISCARD ::PROTOBUF_NAMESPACE_ID::Timestamp* release_start_time();
  ::PROTOBUF_NAMESPACE_ID::Timestamp* mutable_start_time();
  void set_allocated_start_time(::PROTOBUF_NAMESPACE_ID::Timestamp* start_time);
  private:
  const ::PROTOBUF_NAMESPACE_ID::Timestamp& _internal_start_time() const;
  ::PROTOBUF_NAMESPACE_ID::Timestamp* _internal_mutable_start_time();
  public:
  void unsafe_arena_set_allocated_start_time(
      ::PROTOBUF_NAMESPACE_ID::Timestamp* start_time);
  ::PROTOBUF_NAMESPACE_ID::Timestamp* unsafe_arena_release_start_time();

  // .google.protobuf.Timestamp end_time = 3;
  bool has_end_time() const;
  private:
  bool _internal_has_end_time() const;
  public:
  void clear_end_time();
  const ::PROTOBUF_NAMESPACE_ID::Timestamp& end_time() const;
  PROTOBUF_NODISCARD ::PROTOBUF_NAMESPACE_ID::Timestamp* release_end_time();
  ::PROTOBUF_NAMESPACE_ID::Timestamp* mutable_end_time();
  void set_allocated_end_time(::PROTOBUF_NAMESPACE_ID::Timestamp* end_time);
  private:
  const ::PROTOBUF_NAMESPACE_ID::Timestamp& _internal_end_time() const;
  ::PROTOBUF_NAMESPACE_ID::Timestamp* _internal_mutable_end_time();
  public:
  void unsafe_arena_set_allocated_end_time(
      ::PROTOBUF_NAMESPACE_ID::Timestamp* end_time);
  ::PROTOBUF_NAMESPACE_ID::Timestamp* unsafe_arena_release_end_time();

  // int32 progress_percent = 1;
  void clear_progress_percent();
  int32_t progress_percent() const;
  void set_progress_percent(int32_t value);
  private:
  int32_t _internal_progress_percent() const;
  void _internal_set_progress_percent(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:google.spanner.admin.instance.v1.OperationProgress)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::Timestamp* start_time_;
    ::PROTOBUF_NAMESPACE_ID::Timestamp* end_time_;
    int32_t progress_percent_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_google_2fspanner_2fadmin_2finstance_2fv1_2fcommon_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// OperationProgress

// int32 progress_percent = 1;
inline void OperationProgress::clear_progress_percent() {
  _impl_.progress_percent_ = 0;
}
inline int32_t OperationProgress::_internal_progress_percent() const {
  return _impl_.progress_percent_;
}
inline int32_t OperationProgress::progress_percent() const {
  // @@protoc_insertion_point(field_get:google.spanner.admin.instance.v1.OperationProgress.progress_percent)
  return _internal_progress_percent();
}
inline void OperationProgress::_internal_set_progress_percent(int32_t value) {
  
  _impl_.progress_percent_ = value;
}
inline void OperationProgress::set_progress_percent(int32_t value) {
  _internal_set_progress_percent(value);
  // @@protoc_insertion_point(field_set:google.spanner.admin.instance.v1.OperationProgress.progress_percent)
}

// .google.protobuf.Timestamp start_time = 2;
inline bool OperationProgress::_internal_has_start_time() const {
  return this != internal_default_instance() && _impl_.start_time_ != nullptr;
}
inline bool OperationProgress::has_start_time() const {
  return _internal_has_start_time();
}
inline const ::PROTOBUF_NAMESPACE_ID::Timestamp& OperationProgress::_internal_start_time() const {
  const ::PROTOBUF_NAMESPACE_ID::Timestamp* p = _impl_.start_time_;
  return p != nullptr ? *p : reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Timestamp&>(
      ::PROTOBUF_NAMESPACE_ID::_Timestamp_default_instance_);
}
inline const ::PROTOBUF_NAMESPACE_ID::Timestamp& OperationProgress::start_time() const {
  // @@protoc_insertion_point(field_get:google.spanner.admin.instance.v1.OperationProgress.start_time)
  return _internal_start_time();
}
inline void OperationProgress::unsafe_arena_set_allocated_start_time(
    ::PROTOBUF_NAMESPACE_ID::Timestamp* start_time) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.start_time_);
  }
  _impl_.start_time_ = start_time;
  if (start_time) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:google.spanner.admin.instance.v1.OperationProgress.start_time)
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* OperationProgress::release_start_time() {
  
  ::PROTOBUF_NAMESPACE_ID::Timestamp* temp = _impl_.start_time_;
  _impl_.start_time_ = nullptr;
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
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* OperationProgress::unsafe_arena_release_start_time() {
  // @@protoc_insertion_point(field_release:google.spanner.admin.instance.v1.OperationProgress.start_time)
  
  ::PROTOBUF_NAMESPACE_ID::Timestamp* temp = _impl_.start_time_;
  _impl_.start_time_ = nullptr;
  return temp;
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* OperationProgress::_internal_mutable_start_time() {
  
  if (_impl_.start_time_ == nullptr) {
    auto* p = CreateMaybeMessage<::PROTOBUF_NAMESPACE_ID::Timestamp>(GetArenaForAllocation());
    _impl_.start_time_ = p;
  }
  return _impl_.start_time_;
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* OperationProgress::mutable_start_time() {
  ::PROTOBUF_NAMESPACE_ID::Timestamp* _msg = _internal_mutable_start_time();
  // @@protoc_insertion_point(field_mutable:google.spanner.admin.instance.v1.OperationProgress.start_time)
  return _msg;
}
inline void OperationProgress::set_allocated_start_time(::PROTOBUF_NAMESPACE_ID::Timestamp* start_time) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.start_time_);
  }
  if (start_time) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(start_time));
    if (message_arena != submessage_arena) {
      start_time = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, start_time, submessage_arena);
    }
    
  } else {
    
  }
  _impl_.start_time_ = start_time;
  // @@protoc_insertion_point(field_set_allocated:google.spanner.admin.instance.v1.OperationProgress.start_time)
}

// .google.protobuf.Timestamp end_time = 3;
inline bool OperationProgress::_internal_has_end_time() const {
  return this != internal_default_instance() && _impl_.end_time_ != nullptr;
}
inline bool OperationProgress::has_end_time() const {
  return _internal_has_end_time();
}
inline const ::PROTOBUF_NAMESPACE_ID::Timestamp& OperationProgress::_internal_end_time() const {
  const ::PROTOBUF_NAMESPACE_ID::Timestamp* p = _impl_.end_time_;
  return p != nullptr ? *p : reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Timestamp&>(
      ::PROTOBUF_NAMESPACE_ID::_Timestamp_default_instance_);
}
inline const ::PROTOBUF_NAMESPACE_ID::Timestamp& OperationProgress::end_time() const {
  // @@protoc_insertion_point(field_get:google.spanner.admin.instance.v1.OperationProgress.end_time)
  return _internal_end_time();
}
inline void OperationProgress::unsafe_arena_set_allocated_end_time(
    ::PROTOBUF_NAMESPACE_ID::Timestamp* end_time) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.end_time_);
  }
  _impl_.end_time_ = end_time;
  if (end_time) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:google.spanner.admin.instance.v1.OperationProgress.end_time)
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* OperationProgress::release_end_time() {
  
  ::PROTOBUF_NAMESPACE_ID::Timestamp* temp = _impl_.end_time_;
  _impl_.end_time_ = nullptr;
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
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* OperationProgress::unsafe_arena_release_end_time() {
  // @@protoc_insertion_point(field_release:google.spanner.admin.instance.v1.OperationProgress.end_time)
  
  ::PROTOBUF_NAMESPACE_ID::Timestamp* temp = _impl_.end_time_;
  _impl_.end_time_ = nullptr;
  return temp;
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* OperationProgress::_internal_mutable_end_time() {
  
  if (_impl_.end_time_ == nullptr) {
    auto* p = CreateMaybeMessage<::PROTOBUF_NAMESPACE_ID::Timestamp>(GetArenaForAllocation());
    _impl_.end_time_ = p;
  }
  return _impl_.end_time_;
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* OperationProgress::mutable_end_time() {
  ::PROTOBUF_NAMESPACE_ID::Timestamp* _msg = _internal_mutable_end_time();
  // @@protoc_insertion_point(field_mutable:google.spanner.admin.instance.v1.OperationProgress.end_time)
  return _msg;
}
inline void OperationProgress::set_allocated_end_time(::PROTOBUF_NAMESPACE_ID::Timestamp* end_time) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.end_time_);
  }
  if (end_time) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(end_time));
    if (message_arena != submessage_arena) {
      end_time = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, end_time, submessage_arena);
    }
    
  } else {
    
  }
  _impl_.end_time_ = end_time;
  // @@protoc_insertion_point(field_set_allocated:google.spanner.admin.instance.v1.OperationProgress.end_time)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace instance
}  // namespace admin
}  // namespace spanner
}  // namespace google

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_google_2fspanner_2fadmin_2finstance_2fv1_2fcommon_2eproto