// Generated by the protocol buffer compiler.  DO NOT EDIT!
// NO CHECKED-IN PROTOBUF GENCODE
// source: msg.proto
// Protobuf C++ Version: 6.31.1

#include "msg.pb.h"

#include <algorithm>
#include <type_traits>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/generated_message_tctable_impl.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/generated_message_util.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;
namespace Data {

inline constexpr MsgData::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : _cached_size_{0},
        data_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        id_{0} {}

template <typename>
PROTOBUF_CONSTEXPR MsgData::MsgData(::_pbi::ConstantInitialized)
#if defined(PROTOBUF_CUSTOM_VTABLE)
    : ::google::protobuf::Message(MsgData_class_data_.base()),
#else   // PROTOBUF_CUSTOM_VTABLE
    : ::google::protobuf::Message(),
#endif  // PROTOBUF_CUSTOM_VTABLE
      _impl_(::_pbi::ConstantInitialized()) {
}
struct MsgDataDefaultTypeInternal {
  PROTOBUF_CONSTEXPR MsgDataDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~MsgDataDefaultTypeInternal() {}
  union {
    MsgData _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 MsgDataDefaultTypeInternal _MsgData_default_instance_;
}  // namespace Data
static constexpr const ::_pb::EnumDescriptor *PROTOBUF_NONNULL *PROTOBUF_NULLABLE
    file_level_enum_descriptors_msg_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor *PROTOBUF_NONNULL *PROTOBUF_NULLABLE
    file_level_service_descriptors_msg_2eproto = nullptr;
const ::uint32_t
    TableStruct_msg_2eproto::offsets[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
        protodesc_cold) = {
        0x081, // bitmap
        PROTOBUF_FIELD_OFFSET(::Data::MsgData, _impl_._has_bits_),
        5, // hasbit index offset
        PROTOBUF_FIELD_OFFSET(::Data::MsgData, _impl_.id_),
        PROTOBUF_FIELD_OFFSET(::Data::MsgData, _impl_.data_),
        1,
        0,
};

static const ::_pbi::MigrationSchema
    schemas[] ABSL_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
        {0, sizeof(::Data::MsgData)},
};
static const ::_pb::Message* PROTOBUF_NONNULL const file_default_instances[] = {
    &::Data::_MsgData_default_instance_._instance,
};
const char descriptor_table_protodef_msg_2eproto[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
    protodesc_cold) = {
    "\n\tmsg.proto\022\004Data\"#\n\007MsgData\022\n\n\002id\030\001 \001(\005"
    "\022\014\n\004data\030\002 \001(\tb\006proto3"
};
static ::absl::once_flag descriptor_table_msg_2eproto_once;
PROTOBUF_CONSTINIT const ::_pbi::DescriptorTable descriptor_table_msg_2eproto = {
    false,
    false,
    62,
    descriptor_table_protodef_msg_2eproto,
    "msg.proto",
    &descriptor_table_msg_2eproto_once,
    nullptr,
    0,
    1,
    schemas,
    file_default_instances,
    TableStruct_msg_2eproto::offsets,
    file_level_enum_descriptors_msg_2eproto,
    file_level_service_descriptors_msg_2eproto,
};
namespace Data {
// ===================================================================

class MsgData::_Internal {
 public:
  using HasBits =
      decltype(::std::declval<MsgData>()._impl_._has_bits_);
  static constexpr ::int32_t kHasBitsOffset =
      8 * PROTOBUF_FIELD_OFFSET(MsgData, _impl_._has_bits_);
};

MsgData::MsgData(::google::protobuf::Arena* PROTOBUF_NULLABLE arena)
#if defined(PROTOBUF_CUSTOM_VTABLE)
    : ::google::protobuf::Message(arena, MsgData_class_data_.base()) {
#else   // PROTOBUF_CUSTOM_VTABLE
    : ::google::protobuf::Message(arena) {
#endif  // PROTOBUF_CUSTOM_VTABLE
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:Data.MsgData)
}
PROTOBUF_NDEBUG_INLINE MsgData::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* PROTOBUF_NULLABLE arena, const Impl_& from,
    const ::Data::MsgData& from_msg)
      : _has_bits_{from._has_bits_},
        _cached_size_{0},
        data_(arena, from.data_) {}

MsgData::MsgData(
    ::google::protobuf::Arena* PROTOBUF_NULLABLE arena,
    const MsgData& from)
#if defined(PROTOBUF_CUSTOM_VTABLE)
    : ::google::protobuf::Message(arena, MsgData_class_data_.base()) {
#else   // PROTOBUF_CUSTOM_VTABLE
    : ::google::protobuf::Message(arena) {
#endif  // PROTOBUF_CUSTOM_VTABLE
  MsgData* const _this = this;
  (void)_this;
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  new (&_impl_) Impl_(internal_visibility(), arena, from._impl_, from);
  _impl_.id_ = from._impl_.id_;

  // @@protoc_insertion_point(copy_constructor:Data.MsgData)
}
PROTOBUF_NDEBUG_INLINE MsgData::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* PROTOBUF_NULLABLE arena)
      : _cached_size_{0},
        data_(arena) {}

inline void MsgData::SharedCtor(::_pb::Arena* PROTOBUF_NULLABLE arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
  _impl_.id_ = {};
}
MsgData::~MsgData() {
  // @@protoc_insertion_point(destructor:Data.MsgData)
  SharedDtor(*this);
}
inline void MsgData::SharedDtor(MessageLite& self) {
  MsgData& this_ = static_cast<MsgData&>(self);
  this_._internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  ABSL_DCHECK(this_.GetArena() == nullptr);
  this_._impl_.data_.Destroy();
  this_._impl_.~Impl_();
}

inline void* PROTOBUF_NONNULL MsgData::PlacementNew_(
    const void* PROTOBUF_NONNULL, void* PROTOBUF_NONNULL mem,
    ::google::protobuf::Arena* PROTOBUF_NULLABLE arena) {
  return ::new (mem) MsgData(arena);
}
constexpr auto MsgData::InternalNewImpl_() {
  return ::google::protobuf::internal::MessageCreator::CopyInit(sizeof(MsgData),
                                            alignof(MsgData));
}
constexpr auto MsgData::InternalGenerateClassData_() {
  return ::google::protobuf::internal::ClassDataFull{
      ::google::protobuf::internal::ClassData{
          &_MsgData_default_instance_._instance,
          &_table_.header,
          nullptr,  // OnDemandRegisterArenaDtor
          nullptr,  // IsInitialized
          &MsgData::MergeImpl,
          ::google::protobuf::Message::GetNewImpl<MsgData>(),
#if defined(PROTOBUF_CUSTOM_VTABLE)
          &MsgData::SharedDtor,
          ::google::protobuf::Message::GetClearImpl<MsgData>(), &MsgData::ByteSizeLong,
              &MsgData::_InternalSerialize,
#endif  // PROTOBUF_CUSTOM_VTABLE
          PROTOBUF_FIELD_OFFSET(MsgData, _impl_._cached_size_),
          false,
      },
      &MsgData::kDescriptorMethods,
      &descriptor_table_msg_2eproto,
      nullptr,  // tracker
  };
}

PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 const
    ::google::protobuf::internal::ClassDataFull MsgData_class_data_ =
        MsgData::InternalGenerateClassData_();

PROTOBUF_ATTRIBUTE_WEAK const ::google::protobuf::internal::ClassData* PROTOBUF_NONNULL
MsgData::GetClassData() const {
  ::google::protobuf::internal::PrefetchToLocalCache(&MsgData_class_data_);
  ::google::protobuf::internal::PrefetchToLocalCache(MsgData_class_data_.tc_table);
  return MsgData_class_data_.base();
}
PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<1, 2, 0, 25, 2>
MsgData::_table_ = {
  {
    PROTOBUF_FIELD_OFFSET(MsgData, _impl_._has_bits_),
    0, // no _extensions_
    2, 8,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967292,  // skipmap
    offsetof(decltype(_table_), field_entries),
    2,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    MsgData_class_data_.base(),
    nullptr,  // post_loop_handler
    ::_pbi::TcParser::GenericFallback,  // fallback
    #ifdef PROTOBUF_PREFETCH_PARSE_TABLE
    ::_pbi::TcParser::GetTable<::Data::MsgData>(),  // to_prefetch
    #endif  // PROTOBUF_PREFETCH_PARSE_TABLE
  }, {{
    // string data = 2;
    {::_pbi::TcParser::FastUS1,
     {18, 0, 0, PROTOBUF_FIELD_OFFSET(MsgData, _impl_.data_)}},
    // int32 id = 1;
    {::_pbi::TcParser::SingularVarintNoZag1<::uint32_t, offsetof(MsgData, _impl_.id_), 1>(),
     {8, 1, 0, PROTOBUF_FIELD_OFFSET(MsgData, _impl_.id_)}},
  }}, {{
    65535, 65535
  }}, {{
    // int32 id = 1;
    {PROTOBUF_FIELD_OFFSET(MsgData, _impl_.id_), _Internal::kHasBitsOffset + 1, 0,
    (0 | ::_fl::kFcOptional | ::_fl::kInt32)},
    // string data = 2;
    {PROTOBUF_FIELD_OFFSET(MsgData, _impl_.data_), _Internal::kHasBitsOffset + 0, 0,
    (0 | ::_fl::kFcOptional | ::_fl::kUtf8String | ::_fl::kRepAString)},
  }},
  // no aux_entries
  {{
    "\14\0\4\0\0\0\0\0"
    "Data.MsgData"
    "data"
  }},
};
PROTOBUF_NOINLINE void MsgData::Clear() {
// @@protoc_insertion_point(message_clear_start:Data.MsgData)
  ::google::protobuf::internal::TSanWrite(&_impl_);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  if ((cached_has_bits & 0x00000001u) != 0) {
    _impl_.data_.ClearNonDefaultToEmpty();
  }
  _impl_.id_ = 0;
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

#if defined(PROTOBUF_CUSTOM_VTABLE)
::uint8_t* PROTOBUF_NONNULL MsgData::_InternalSerialize(
    const ::google::protobuf::MessageLite& base, ::uint8_t* PROTOBUF_NONNULL target,
    ::google::protobuf::io::EpsCopyOutputStream* PROTOBUF_NONNULL stream) {
  const MsgData& this_ = static_cast<const MsgData&>(base);
#else   // PROTOBUF_CUSTOM_VTABLE
::uint8_t* PROTOBUF_NONNULL MsgData::_InternalSerialize(
    ::uint8_t* PROTOBUF_NONNULL target,
    ::google::protobuf::io::EpsCopyOutputStream* PROTOBUF_NONNULL stream) const {
  const MsgData& this_ = *this;
#endif  // PROTOBUF_CUSTOM_VTABLE
  // @@protoc_insertion_point(serialize_to_array_start:Data.MsgData)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  // int32 id = 1;
  if ((this_._impl_._has_bits_[0] & 0x00000002u) != 0) {
    if (this_._internal_id() != 0) {
      target =
          ::google::protobuf::internal::WireFormatLite::WriteInt32ToArrayWithField<1>(
              stream, this_._internal_id(), target);
    }
  }

  // string data = 2;
  if ((this_._impl_._has_bits_[0] & 0x00000001u) != 0) {
    if (!this_._internal_data().empty()) {
      const ::std::string& _s = this_._internal_data();
      ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
          _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Data.MsgData.data");
      target = stream->WriteStringMaybeAliased(2, _s, target);
    }
  }

  if (ABSL_PREDICT_FALSE(this_._internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            this_._internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Data.MsgData)
  return target;
}

#if defined(PROTOBUF_CUSTOM_VTABLE)
::size_t MsgData::ByteSizeLong(const MessageLite& base) {
  const MsgData& this_ = static_cast<const MsgData&>(base);
#else   // PROTOBUF_CUSTOM_VTABLE
::size_t MsgData::ByteSizeLong() const {
  const MsgData& this_ = *this;
#endif  // PROTOBUF_CUSTOM_VTABLE
  // @@protoc_insertion_point(message_byte_size_start:Data.MsgData)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void)cached_has_bits;

  ::_pbi::Prefetch5LinesFrom7Lines(&this_);
  cached_has_bits = this_._impl_._has_bits_[0];
  if ((cached_has_bits & 0x00000003u) != 0) {
    // string data = 2;
    if ((cached_has_bits & 0x00000001u) != 0) {
      if (!this_._internal_data().empty()) {
        total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                        this_._internal_data());
      }
    }
    // int32 id = 1;
    if ((cached_has_bits & 0x00000002u) != 0) {
      if (this_._internal_id() != 0) {
        total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(
            this_._internal_id());
      }
    }
  }
  return this_.MaybeComputeUnknownFieldsSize(total_size,
                                             &this_._impl_._cached_size_);
}

void MsgData::MergeImpl(::google::protobuf::MessageLite& to_msg, const ::google::protobuf::MessageLite& from_msg) {
  auto* const _this = static_cast<MsgData*>(&to_msg);
  auto& from = static_cast<const MsgData&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:Data.MsgData)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._impl_._has_bits_[0];
  if ((cached_has_bits & 0x00000003u) != 0) {
    if ((cached_has_bits & 0x00000001u) != 0) {
      if (!from._internal_data().empty()) {
        _this->_internal_set_data(from._internal_data());
      } else {
        if (_this->_impl_.data_.IsDefault()) {
          _this->_internal_set_data("");
        }
      }
    }
    if ((cached_has_bits & 0x00000002u) != 0) {
      if (from._internal_id() != 0) {
        _this->_impl_.id_ = from._impl_.id_;
      }
    }
  }
  _this->_impl_._has_bits_[0] |= cached_has_bits;
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void MsgData::CopyFrom(const MsgData& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Data.MsgData)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}


void MsgData::InternalSwap(MsgData* PROTOBUF_RESTRICT PROTOBUF_NONNULL other) {
  using ::std::swap;
  auto* arena = GetArena();
  ABSL_DCHECK_EQ(arena, other->GetArena());
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.data_, &other->_impl_.data_, arena);
  swap(_impl_.id_, other->_impl_.id_);
}

::google::protobuf::Metadata MsgData::GetMetadata() const {
  return ::google::protobuf::Message::GetMetadataImpl(GetClassData()->full());
}
// @@protoc_insertion_point(namespace_scope)
}  // namespace Data
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::std::false_type
    _static_init2_ [[maybe_unused]] =
        (::_pbi::AddDescriptors(&descriptor_table_msg_2eproto),
         ::std::false_type{});
#include "google/protobuf/port_undef.inc"
