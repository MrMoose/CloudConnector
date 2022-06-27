// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/api/error_reason.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_google_2fapi_2ferror_5freason_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_google_2fapi_2ferror_5freason_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3019000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3019004 < PROTOBUF_MIN_PROTOC_VERSION
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
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_google_2fapi_2ferror_5freason_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_google_2fapi_2ferror_5freason_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_google_2fapi_2ferror_5freason_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE
namespace google {
namespace api {

enum ErrorReason : int {
  ERROR_REASON_UNSPECIFIED = 0,
  SERVICE_DISABLED = 1,
  BILLING_DISABLED = 2,
  API_KEY_INVALID = 3,
  API_KEY_SERVICE_BLOCKED = 4,
  API_KEY_HTTP_REFERRER_BLOCKED = 7,
  API_KEY_IP_ADDRESS_BLOCKED = 8,
  API_KEY_ANDROID_APP_BLOCKED = 9,
  API_KEY_IOS_APP_BLOCKED = 13,
  RATE_LIMIT_EXCEEDED = 5,
  RESOURCE_QUOTA_EXCEEDED = 6,
  LOCATION_TAX_POLICY_VIOLATED = 10,
  USER_PROJECT_DENIED = 11,
  CONSUMER_SUSPENDED = 12,
  CONSUMER_INVALID = 14,
  SECURITY_POLICY_VIOLATED = 15,
  ACCESS_TOKEN_EXPIRED = 16,
  ACCESS_TOKEN_SCOPE_INSUFFICIENT = 17,
  ACCOUNT_STATE_INVALID = 18,
  ACCESS_TOKEN_TYPE_UNSUPPORTED = 19,
  ErrorReason_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  ErrorReason_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool ErrorReason_IsValid(int value);
constexpr ErrorReason ErrorReason_MIN = ERROR_REASON_UNSPECIFIED;
constexpr ErrorReason ErrorReason_MAX = ACCESS_TOKEN_TYPE_UNSUPPORTED;
constexpr int ErrorReason_ARRAYSIZE = ErrorReason_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ErrorReason_descriptor();
template<typename T>
inline const std::string& ErrorReason_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ErrorReason>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ErrorReason_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ErrorReason_descriptor(), enum_t_value);
}
inline bool ErrorReason_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ErrorReason* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ErrorReason>(
    ErrorReason_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace api
}  // namespace google

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::google::api::ErrorReason> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::google::api::ErrorReason>() {
  return ::google::api::ErrorReason_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_google_2fapi_2ferror_5freason_2eproto