#include "AST/Type.hpp"
#include "sona/util.hpp"

namespace ckx {

char const* BuiltinType::GetTypeName() const noexcept {
    switch (GetTypeId()) {
    case TypeId::ty_u8: return  "vu8";
    case TypeId::ty_u16: return "vu16";
    case TypeId::ty_u32: return "vu32";
    case TypeId::ty_u64: return "vu64";
    case TypeId::ty_i8: return "vi8";
    case TypeId::ty_i16: return "vi16";
    case TypeId::ty_i32: return "vi32";
    case TypeId::ty_i64: return "vi64";
    case TypeId::ty_bool: return "bool";
    case TypeId::ty_nil: return "nil";
    case TypeId::ty_void: return "void";

    default: sona_unreachable();
    }
    return "";
}

bool BuiltinType::IsNumeric() const noexcept {
    return (GetTypeId() >= TypeId::ty_u8) && (GetTypeId() <= TypeId::ty_r64);
}

bool BuiltinType::IsIntegral() const noexcept {
    return (GetTypeId() >= TypeId::ty_u8) && (GetTypeId() <= TypeId::ty_i64);
}

bool BuiltinType::IsSigned() const noexcept {
    return IsIntegral() && GetTypeId() >= TypeId::ty_i8;
}

bool BuiltinType::IsUnsigned() const noexcept {
    return IsIntegral() && !(IsSigned());
}

BuiltinType BuiltinType::MakeSigned(BuiltinType const& that) noexcept {
    sona_assert(that.IsUnsigned());
    return BuiltinType(TypeId((std::int8_t)that.GetTypeId()
                              + (std::int8_t)TypeId::ty_i8
                              - (std::int8_t)TypeId::ty_u8));
}

BuiltinType BuiltinType::MakeUnsigned(BuiltinType const& that) noexcept {
    sona_assert(that.IsSigned());
    return BuiltinType(TypeId((std::int8_t)that.GetTypeId()
                              - (std::int8_t)TypeId::ty_i8
                              + (std::int8_t)TypeId::ty_u8));
}

} // namespace ckx
