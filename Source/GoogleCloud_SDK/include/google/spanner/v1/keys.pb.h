// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/spanner/v1/keys.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_google_2fspanner_2fv1_2fkeys_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_google_2fspanner_2fv1_2fkeys_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3015000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3015008 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include <google/protobuf/struct.pb.h>
#include "google/api/annotations.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_google_2fspanner_2fv1_2fkeys_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_google_2fspanner_2fv1_2fkeys_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_google_2fspanner_2fv1_2fkeys_2eproto;
::PROTOBUF_NAMESPACE_ID::Metadata descriptor_table_google_2fspanner_2fv1_2fkeys_2eproto_metadata_getter(int index);
namespace google {
namespace spanner {
namespace v1 {
class KeyRange;
struct KeyRangeDefaultTypeInternal;
extern KeyRangeDefaultTypeInternal _KeyRange_default_instance_;
class KeySet;
struct KeySetDefaultTypeInternal;
extern KeySetDefaultTypeInternal _KeySet_default_instance_;
}  // namespace v1
}  // namespace spanner
}  // namespace google
PROTOBUF_NAMESPACE_OPEN
template<> ::google::spanner::v1::KeyRange* Arena::CreateMaybeMessage<::google::spanner::v1::KeyRange>(Arena*);
template<> ::google::spanner::v1::KeySet* Arena::CreateMaybeMessage<::google::spanner::v1::KeySet>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace google {
namespace spanner {
namespace v1 {

// ===================================================================

class KeyRange PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:google.spanner.v1.KeyRange) */ {
 public:
  inline KeyRange() : KeyRange(nullptr) {}
  virtual ~KeyRange();
  explicit constexpr KeyRange(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  KeyRange(const KeyRange& from);
  KeyRange(KeyRange&& from) noexcept
    : KeyRange() {
    *this = ::std::move(from);
  }

  inline KeyRange& operator=(const KeyRange& from) {
    CopyFrom(from);
    return *this;
  }
  inline KeyRange& operator=(KeyRange&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const KeyRange& default_instance() {
    return *internal_default_instance();
  }
  enum StartKeyTypeCase {
    kStartClosed = 1,
    kStartOpen = 2,
    START_KEY_TYPE_NOT_SET = 0,
  };

  enum EndKeyTypeCase {
    kEndClosed = 3,
    kEndOpen = 4,
    END_KEY_TYPE_NOT_SET = 0,
  };

  static inline const KeyRange* internal_default_instance() {
    return reinterpret_cast<const KeyRange*>(
               &_KeyRange_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(KeyRange& a, KeyRange& b) {
    a.Swap(&b);
  }
  inline void Swap(KeyRange* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(KeyRange* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline KeyRange* New() const final {
    return CreateMaybeMessage<KeyRange>(nullptr);
  }

  KeyRange* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<KeyRange>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const KeyRange& from);
  void MergeFrom(const KeyRange& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(KeyRange* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "google.spanner.v1.KeyRange";
  }
  protected:
  explicit KeyRange(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    return ::descriptor_table_google_2fspanner_2fv1_2fkeys_2eproto_metadata_getter(kIndexInFileMessages);
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kStartClosedFieldNumber = 1,
    kStartOpenFieldNumber = 2,
    kEndClosedFieldNumber = 3,
    kEndOpenFieldNumber = 4,
  };
  // .google.protobuf.ListValue start_closed = 1;
  bool has_start_closed() const;
  private:
  bool _internal_has_start_closed() const;
  public:
  void clear_start_closed();
  const PROTOBUF_NAMESPACE_ID::ListValue& start_closed() const;
  PROTOBUF_NAMESPACE_ID::ListValue* release_start_closed();
  PROTOBUF_NAMESPACE_ID::ListValue* mutable_start_closed();
  void set_allocated_start_closed(PROTOBUF_NAMESPACE_ID::ListValue* start_closed);
  private:
  const PROTOBUF_NAMESPACE_ID::ListValue& _internal_start_closed() const;
  PROTOBUF_NAMESPACE_ID::ListValue* _internal_mutable_start_closed();
  public:
  void unsafe_arena_set_allocated_start_closed(
      PROTOBUF_NAMESPACE_ID::ListValue* start_closed);
  PROTOBUF_NAMESPACE_ID::ListValue* unsafe_arena_release_start_closed();

  // .google.protobuf.ListValue start_open = 2;
  bool has_start_open() const;
  private:
  bool _internal_has_start_open() const;
  public:
  void clear_start_open();
  const PROTOBUF_NAMESPACE_ID::ListValue& start_open() const;
  PROTOBUF_NAMESPACE_ID::ListValue* release_start_open();
  PROTOBUF_NAMESPACE_ID::ListValue* mutable_start_open();
  void set_allocated_start_open(PROTOBUF_NAMESPACE_ID::ListValue* start_open);
  private:
  const PROTOBUF_NAMESPACE_ID::ListValue& _internal_start_open() const;
  PROTOBUF_NAMESPACE_ID::ListValue* _internal_mutable_start_open();
  public:
  void unsafe_arena_set_allocated_start_open(
      PROTOBUF_NAMESPACE_ID::ListValue* start_open);
  PROTOBUF_NAMESPACE_ID::ListValue* unsafe_arena_release_start_open();

  // .google.protobuf.ListValue end_closed = 3;
  bool has_end_closed() const;
  private:
  bool _internal_has_end_closed() const;
  public:
  void clear_end_closed();
  const PROTOBUF_NAMESPACE_ID::ListValue& end_closed() const;
  PROTOBUF_NAMESPACE_ID::ListValue* release_end_closed();
  PROTOBUF_NAMESPACE_ID::ListValue* mutable_end_closed();
  void set_allocated_end_closed(PROTOBUF_NAMESPACE_ID::ListValue* end_closed);
  private:
  const PROTOBUF_NAMESPACE_ID::ListValue& _internal_end_closed() const;
  PROTOBUF_NAMESPACE_ID::ListValue* _internal_mutable_end_closed();
  public:
  void unsafe_arena_set_allocated_end_closed(
      PROTOBUF_NAMESPACE_ID::ListValue* end_closed);
  PROTOBUF_NAMESPACE_ID::ListValue* unsafe_arena_release_end_closed();

  // .google.protobuf.ListValue end_open = 4;
  bool has_end_open() const;
  private:
  bool _internal_has_end_open() const;
  public:
  void clear_end_open();
  const PROTOBUF_NAMESPACE_ID::ListValue& end_open() const;
  PROTOBUF_NAMESPACE_ID::ListValue* release_end_open();
  PROTOBUF_NAMESPACE_ID::ListValue* mutable_end_open();
  void set_allocated_end_open(PROTOBUF_NAMESPACE_ID::ListValue* end_open);
  private:
  const PROTOBUF_NAMESPACE_ID::ListValue& _internal_end_open() const;
  PROTOBUF_NAMESPACE_ID::ListValue* _internal_mutable_end_open();
  public:
  void unsafe_arena_set_allocated_end_open(
      PROTOBUF_NAMESPACE_ID::ListValue* end_open);
  PROTOBUF_NAMESPACE_ID::ListValue* unsafe_arena_release_end_open();

  void clear_start_key_type();
  StartKeyTypeCase start_key_type_case() const;
  void clear_end_key_type();
  EndKeyTypeCase end_key_type_case() const;
  // @@protoc_insertion_point(class_scope:google.spanner.v1.KeyRange)
 private:
  class _Internal;
  void set_has_start_closed();
  void set_has_start_open();
  void set_has_end_closed();
  void set_has_end_open();

  inline bool has_start_key_type() const;
  inline void clear_has_start_key_type();

  inline bool has_end_key_type() const;
  inline void clear_has_end_key_type();

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  union StartKeyTypeUnion {
    constexpr StartKeyTypeUnion() : _constinit_{} {}
      ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized _constinit_;
    PROTOBUF_NAMESPACE_ID::ListValue* start_closed_;
    PROTOBUF_NAMESPACE_ID::ListValue* start_open_;
  } start_key_type_;
  union EndKeyTypeUnion {
    constexpr EndKeyTypeUnion() : _constinit_{} {}
      ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized _constinit_;
    PROTOBUF_NAMESPACE_ID::ListValue* end_closed_;
    PROTOBUF_NAMESPACE_ID::ListValue* end_open_;
  } end_key_type_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::uint32 _oneof_case_[2];

  friend struct ::TableStruct_google_2fspanner_2fv1_2fkeys_2eproto;
};
// -------------------------------------------------------------------

class KeySet PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:google.spanner.v1.KeySet) */ {
 public:
  inline KeySet() : KeySet(nullptr) {}
  virtual ~KeySet();
  explicit constexpr KeySet(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  KeySet(const KeySet& from);
  KeySet(KeySet&& from) noexcept
    : KeySet() {
    *this = ::std::move(from);
  }

  inline KeySet& operator=(const KeySet& from) {
    CopyFrom(from);
    return *this;
  }
  inline KeySet& operator=(KeySet&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const KeySet& default_instance() {
    return *internal_default_instance();
  }
  static inline const KeySet* internal_default_instance() {
    return reinterpret_cast<const KeySet*>(
               &_KeySet_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(KeySet& a, KeySet& b) {
    a.Swap(&b);
  }
  inline void Swap(KeySet* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(KeySet* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline KeySet* New() const final {
    return CreateMaybeMessage<KeySet>(nullptr);
  }

  KeySet* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<KeySet>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const KeySet& from);
  void MergeFrom(const KeySet& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(KeySet* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "google.spanner.v1.KeySet";
  }
  protected:
  explicit KeySet(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    return ::descriptor_table_google_2fspanner_2fv1_2fkeys_2eproto_metadata_getter(kIndexInFileMessages);
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kKeysFieldNumber = 1,
    kRangesFieldNumber = 2,
    kAllFieldNumber = 3,
  };
  // repeated .google.protobuf.ListValue keys = 1;
  int keys_size() const;
  private:
  int _internal_keys_size() const;
  public:
  void clear_keys();
  PROTOBUF_NAMESPACE_ID::ListValue* mutable_keys(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< PROTOBUF_NAMESPACE_ID::ListValue >*
      mutable_keys();
  private:
  const PROTOBUF_NAMESPACE_ID::ListValue& _internal_keys(int index) const;
  PROTOBUF_NAMESPACE_ID::ListValue* _internal_add_keys();
  public:
  const PROTOBUF_NAMESPACE_ID::ListValue& keys(int index) const;
  PROTOBUF_NAMESPACE_ID::ListValue* add_keys();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< PROTOBUF_NAMESPACE_ID::ListValue >&
      keys() const;

  // repeated .google.spanner.v1.KeyRange ranges = 2;
  int ranges_size() const;
  private:
  int _internal_ranges_size() const;
  public:
  void clear_ranges();
  ::google::spanner::v1::KeyRange* mutable_ranges(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::google::spanner::v1::KeyRange >*
      mutable_ranges();
  private:
  const ::google::spanner::v1::KeyRange& _internal_ranges(int index) const;
  ::google::spanner::v1::KeyRange* _internal_add_ranges();
  public:
  const ::google::spanner::v1::KeyRange& ranges(int index) const;
  ::google::spanner::v1::KeyRange* add_ranges();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::google::spanner::v1::KeyRange >&
      ranges() const;

  // bool all = 3;
  void clear_all();
  bool all() const;
  void set_all(bool value);
  private:
  bool _internal_all() const;
  void _internal_set_all(bool value);
  public:

  // @@protoc_insertion_point(class_scope:google.spanner.v1.KeySet)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< PROTOBUF_NAMESPACE_ID::ListValue > keys_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::google::spanner::v1::KeyRange > ranges_;
  bool all_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_google_2fspanner_2fv1_2fkeys_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// KeyRange

// .google.protobuf.ListValue start_closed = 1;
inline bool KeyRange::_internal_has_start_closed() const {
  return start_key_type_case() == kStartClosed;
}
inline bool KeyRange::has_start_closed() const {
  return _internal_has_start_closed();
}
inline void KeyRange::set_has_start_closed() {
  _oneof_case_[0] = kStartClosed;
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::release_start_closed() {
  // @@protoc_insertion_point(field_release:google.spanner.v1.KeyRange.start_closed)
  if (_internal_has_start_closed()) {
    clear_has_start_key_type();
      PROTOBUF_NAMESPACE_ID::ListValue* temp = start_key_type_.start_closed_;
    if (GetArena() != nullptr) {
      temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
    }
    start_key_type_.start_closed_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline const PROTOBUF_NAMESPACE_ID::ListValue& KeyRange::_internal_start_closed() const {
  return _internal_has_start_closed()
      ? *start_key_type_.start_closed_
      : reinterpret_cast< PROTOBUF_NAMESPACE_ID::ListValue&>(PROTOBUF_NAMESPACE_ID::_ListValue_default_instance_);
}
inline const PROTOBUF_NAMESPACE_ID::ListValue& KeyRange::start_closed() const {
  // @@protoc_insertion_point(field_get:google.spanner.v1.KeyRange.start_closed)
  return _internal_start_closed();
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::unsafe_arena_release_start_closed() {
  // @@protoc_insertion_point(field_unsafe_arena_release:google.spanner.v1.KeyRange.start_closed)
  if (_internal_has_start_closed()) {
    clear_has_start_key_type();
    PROTOBUF_NAMESPACE_ID::ListValue* temp = start_key_type_.start_closed_;
    start_key_type_.start_closed_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline void KeyRange::unsafe_arena_set_allocated_start_closed(PROTOBUF_NAMESPACE_ID::ListValue* start_closed) {
  clear_start_key_type();
  if (start_closed) {
    set_has_start_closed();
    start_key_type_.start_closed_ = start_closed;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:google.spanner.v1.KeyRange.start_closed)
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::_internal_mutable_start_closed() {
  if (!_internal_has_start_closed()) {
    clear_start_key_type();
    set_has_start_closed();
    start_key_type_.start_closed_ = CreateMaybeMessage< PROTOBUF_NAMESPACE_ID::ListValue >(GetArena());
  }
  return start_key_type_.start_closed_;
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::mutable_start_closed() {
  // @@protoc_insertion_point(field_mutable:google.spanner.v1.KeyRange.start_closed)
  return _internal_mutable_start_closed();
}

// .google.protobuf.ListValue start_open = 2;
inline bool KeyRange::_internal_has_start_open() const {
  return start_key_type_case() == kStartOpen;
}
inline bool KeyRange::has_start_open() const {
  return _internal_has_start_open();
}
inline void KeyRange::set_has_start_open() {
  _oneof_case_[0] = kStartOpen;
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::release_start_open() {
  // @@protoc_insertion_point(field_release:google.spanner.v1.KeyRange.start_open)
  if (_internal_has_start_open()) {
    clear_has_start_key_type();
      PROTOBUF_NAMESPACE_ID::ListValue* temp = start_key_type_.start_open_;
    if (GetArena() != nullptr) {
      temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
    }
    start_key_type_.start_open_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline const PROTOBUF_NAMESPACE_ID::ListValue& KeyRange::_internal_start_open() const {
  return _internal_has_start_open()
      ? *start_key_type_.start_open_
      : reinterpret_cast< PROTOBUF_NAMESPACE_ID::ListValue&>(PROTOBUF_NAMESPACE_ID::_ListValue_default_instance_);
}
inline const PROTOBUF_NAMESPACE_ID::ListValue& KeyRange::start_open() const {
  // @@protoc_insertion_point(field_get:google.spanner.v1.KeyRange.start_open)
  return _internal_start_open();
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::unsafe_arena_release_start_open() {
  // @@protoc_insertion_point(field_unsafe_arena_release:google.spanner.v1.KeyRange.start_open)
  if (_internal_has_start_open()) {
    clear_has_start_key_type();
    PROTOBUF_NAMESPACE_ID::ListValue* temp = start_key_type_.start_open_;
    start_key_type_.start_open_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline void KeyRange::unsafe_arena_set_allocated_start_open(PROTOBUF_NAMESPACE_ID::ListValue* start_open) {
  clear_start_key_type();
  if (start_open) {
    set_has_start_open();
    start_key_type_.start_open_ = start_open;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:google.spanner.v1.KeyRange.start_open)
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::_internal_mutable_start_open() {
  if (!_internal_has_start_open()) {
    clear_start_key_type();
    set_has_start_open();
    start_key_type_.start_open_ = CreateMaybeMessage< PROTOBUF_NAMESPACE_ID::ListValue >(GetArena());
  }
  return start_key_type_.start_open_;
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::mutable_start_open() {
  // @@protoc_insertion_point(field_mutable:google.spanner.v1.KeyRange.start_open)
  return _internal_mutable_start_open();
}

// .google.protobuf.ListValue end_closed = 3;
inline bool KeyRange::_internal_has_end_closed() const {
  return end_key_type_case() == kEndClosed;
}
inline bool KeyRange::has_end_closed() const {
  return _internal_has_end_closed();
}
inline void KeyRange::set_has_end_closed() {
  _oneof_case_[1] = kEndClosed;
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::release_end_closed() {
  // @@protoc_insertion_point(field_release:google.spanner.v1.KeyRange.end_closed)
  if (_internal_has_end_closed()) {
    clear_has_end_key_type();
      PROTOBUF_NAMESPACE_ID::ListValue* temp = end_key_type_.end_closed_;
    if (GetArena() != nullptr) {
      temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
    }
    end_key_type_.end_closed_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline const PROTOBUF_NAMESPACE_ID::ListValue& KeyRange::_internal_end_closed() const {
  return _internal_has_end_closed()
      ? *end_key_type_.end_closed_
      : reinterpret_cast< PROTOBUF_NAMESPACE_ID::ListValue&>(PROTOBUF_NAMESPACE_ID::_ListValue_default_instance_);
}
inline const PROTOBUF_NAMESPACE_ID::ListValue& KeyRange::end_closed() const {
  // @@protoc_insertion_point(field_get:google.spanner.v1.KeyRange.end_closed)
  return _internal_end_closed();
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::unsafe_arena_release_end_closed() {
  // @@protoc_insertion_point(field_unsafe_arena_release:google.spanner.v1.KeyRange.end_closed)
  if (_internal_has_end_closed()) {
    clear_has_end_key_type();
    PROTOBUF_NAMESPACE_ID::ListValue* temp = end_key_type_.end_closed_;
    end_key_type_.end_closed_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline void KeyRange::unsafe_arena_set_allocated_end_closed(PROTOBUF_NAMESPACE_ID::ListValue* end_closed) {
  clear_end_key_type();
  if (end_closed) {
    set_has_end_closed();
    end_key_type_.end_closed_ = end_closed;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:google.spanner.v1.KeyRange.end_closed)
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::_internal_mutable_end_closed() {
  if (!_internal_has_end_closed()) {
    clear_end_key_type();
    set_has_end_closed();
    end_key_type_.end_closed_ = CreateMaybeMessage< PROTOBUF_NAMESPACE_ID::ListValue >(GetArena());
  }
  return end_key_type_.end_closed_;
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::mutable_end_closed() {
  // @@protoc_insertion_point(field_mutable:google.spanner.v1.KeyRange.end_closed)
  return _internal_mutable_end_closed();
}

// .google.protobuf.ListValue end_open = 4;
inline bool KeyRange::_internal_has_end_open() const {
  return end_key_type_case() == kEndOpen;
}
inline bool KeyRange::has_end_open() const {
  return _internal_has_end_open();
}
inline void KeyRange::set_has_end_open() {
  _oneof_case_[1] = kEndOpen;
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::release_end_open() {
  // @@protoc_insertion_point(field_release:google.spanner.v1.KeyRange.end_open)
  if (_internal_has_end_open()) {
    clear_has_end_key_type();
      PROTOBUF_NAMESPACE_ID::ListValue* temp = end_key_type_.end_open_;
    if (GetArena() != nullptr) {
      temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
    }
    end_key_type_.end_open_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline const PROTOBUF_NAMESPACE_ID::ListValue& KeyRange::_internal_end_open() const {
  return _internal_has_end_open()
      ? *end_key_type_.end_open_
      : reinterpret_cast< PROTOBUF_NAMESPACE_ID::ListValue&>(PROTOBUF_NAMESPACE_ID::_ListValue_default_instance_);
}
inline const PROTOBUF_NAMESPACE_ID::ListValue& KeyRange::end_open() const {
  // @@protoc_insertion_point(field_get:google.spanner.v1.KeyRange.end_open)
  return _internal_end_open();
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::unsafe_arena_release_end_open() {
  // @@protoc_insertion_point(field_unsafe_arena_release:google.spanner.v1.KeyRange.end_open)
  if (_internal_has_end_open()) {
    clear_has_end_key_type();
    PROTOBUF_NAMESPACE_ID::ListValue* temp = end_key_type_.end_open_;
    end_key_type_.end_open_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline void KeyRange::unsafe_arena_set_allocated_end_open(PROTOBUF_NAMESPACE_ID::ListValue* end_open) {
  clear_end_key_type();
  if (end_open) {
    set_has_end_open();
    end_key_type_.end_open_ = end_open;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:google.spanner.v1.KeyRange.end_open)
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::_internal_mutable_end_open() {
  if (!_internal_has_end_open()) {
    clear_end_key_type();
    set_has_end_open();
    end_key_type_.end_open_ = CreateMaybeMessage< PROTOBUF_NAMESPACE_ID::ListValue >(GetArena());
  }
  return end_key_type_.end_open_;
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeyRange::mutable_end_open() {
  // @@protoc_insertion_point(field_mutable:google.spanner.v1.KeyRange.end_open)
  return _internal_mutable_end_open();
}

inline bool KeyRange::has_start_key_type() const {
  return start_key_type_case() != START_KEY_TYPE_NOT_SET;
}
inline void KeyRange::clear_has_start_key_type() {
  _oneof_case_[0] = START_KEY_TYPE_NOT_SET;
}
inline bool KeyRange::has_end_key_type() const {
  return end_key_type_case() != END_KEY_TYPE_NOT_SET;
}
inline void KeyRange::clear_has_end_key_type() {
  _oneof_case_[1] = END_KEY_TYPE_NOT_SET;
}
inline KeyRange::StartKeyTypeCase KeyRange::start_key_type_case() const {
  return KeyRange::StartKeyTypeCase(_oneof_case_[0]);
}
inline KeyRange::EndKeyTypeCase KeyRange::end_key_type_case() const {
  return KeyRange::EndKeyTypeCase(_oneof_case_[1]);
}
// -------------------------------------------------------------------

// KeySet

// repeated .google.protobuf.ListValue keys = 1;
inline int KeySet::_internal_keys_size() const {
  return keys_.size();
}
inline int KeySet::keys_size() const {
  return _internal_keys_size();
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeySet::mutable_keys(int index) {
  // @@protoc_insertion_point(field_mutable:google.spanner.v1.KeySet.keys)
  return keys_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< PROTOBUF_NAMESPACE_ID::ListValue >*
KeySet::mutable_keys() {
  // @@protoc_insertion_point(field_mutable_list:google.spanner.v1.KeySet.keys)
  return &keys_;
}
inline const PROTOBUF_NAMESPACE_ID::ListValue& KeySet::_internal_keys(int index) const {
  return keys_.Get(index);
}
inline const PROTOBUF_NAMESPACE_ID::ListValue& KeySet::keys(int index) const {
  // @@protoc_insertion_point(field_get:google.spanner.v1.KeySet.keys)
  return _internal_keys(index);
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeySet::_internal_add_keys() {
  return keys_.Add();
}
inline PROTOBUF_NAMESPACE_ID::ListValue* KeySet::add_keys() {
  // @@protoc_insertion_point(field_add:google.spanner.v1.KeySet.keys)
  return _internal_add_keys();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< PROTOBUF_NAMESPACE_ID::ListValue >&
KeySet::keys() const {
  // @@protoc_insertion_point(field_list:google.spanner.v1.KeySet.keys)
  return keys_;
}

// repeated .google.spanner.v1.KeyRange ranges = 2;
inline int KeySet::_internal_ranges_size() const {
  return ranges_.size();
}
inline int KeySet::ranges_size() const {
  return _internal_ranges_size();
}
inline void KeySet::clear_ranges() {
  ranges_.Clear();
}
inline ::google::spanner::v1::KeyRange* KeySet::mutable_ranges(int index) {
  // @@protoc_insertion_point(field_mutable:google.spanner.v1.KeySet.ranges)
  return ranges_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::google::spanner::v1::KeyRange >*
KeySet::mutable_ranges() {
  // @@protoc_insertion_point(field_mutable_list:google.spanner.v1.KeySet.ranges)
  return &ranges_;
}
inline const ::google::spanner::v1::KeyRange& KeySet::_internal_ranges(int index) const {
  return ranges_.Get(index);
}
inline const ::google::spanner::v1::KeyRange& KeySet::ranges(int index) const {
  // @@protoc_insertion_point(field_get:google.spanner.v1.KeySet.ranges)
  return _internal_ranges(index);
}
inline ::google::spanner::v1::KeyRange* KeySet::_internal_add_ranges() {
  return ranges_.Add();
}
inline ::google::spanner::v1::KeyRange* KeySet::add_ranges() {
  // @@protoc_insertion_point(field_add:google.spanner.v1.KeySet.ranges)
  return _internal_add_ranges();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::google::spanner::v1::KeyRange >&
KeySet::ranges() const {
  // @@protoc_insertion_point(field_list:google.spanner.v1.KeySet.ranges)
  return ranges_;
}

// bool all = 3;
inline void KeySet::clear_all() {
  all_ = false;
}
inline bool KeySet::_internal_all() const {
  return all_;
}
inline bool KeySet::all() const {
  // @@protoc_insertion_point(field_get:google.spanner.v1.KeySet.all)
  return _internal_all();
}
inline void KeySet::_internal_set_all(bool value) {
  
  all_ = value;
}
inline void KeySet::set_all(bool value) {
  _internal_set_all(value);
  // @@protoc_insertion_point(field_set:google.spanner.v1.KeySet.all)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace spanner
}  // namespace google

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_google_2fspanner_2fv1_2fkeys_2eproto
