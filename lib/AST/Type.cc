#include "AST/Type.hpp"
#include "sona/util.hpp"

#include <algorithm>
#include <type_traits>

namespace ckx {

using std::size_t;
template <typename T>
using DefaultHash = std::hash<T>;

size_t Type::GetHash() const noexcept {
    using NumericTypeId = std::underlying_type_t<TypeId>;
    return DefaultHash<NumericTypeId>()(
        static_cast<NumericTypeId>(GetTypeId()));
}

char const* BuiltinType::GetTypeName() const noexcept {
    switch (GetTypeId()) {
    case TypeId::TI_u8: return  "vu8";
    case TypeId::TI_u16: return "vu16";
    case TypeId::TI_u32: return "vu32";
    case TypeId::TI_u64: return "vu64";
    case TypeId::TI_i8: return "vi8";
    case TypeId::TI_i16: return "vi16";
    case TypeId::TI_i32: return "vi32";
    case TypeId::TI_i64: return "vi64";
    case TypeId::TI_bool: return "bool";
    case TypeId::TI_nil: return "nil";
    case TypeId::TI_void: return "void";

    default: sona_unreachable();
    }
    return "";
}

bool BuiltinType::IsNumeric() const noexcept {
    return (GetTypeId() >= TypeId::TI_u8) && (GetTypeId() <= TypeId::TI_r64);
}

bool BuiltinType::IsIntegral() const noexcept {
    return (GetTypeId() >= TypeId::TI_u8) && (GetTypeId() <= TypeId::TI_i64);
}

bool BuiltinType::IsSigned() const noexcept {
    return IsIntegral() && GetTypeId() >= TypeId::TI_i8;
}

bool BuiltinType::IsUnsigned() const noexcept {
    return IsIntegral() && !(IsSigned());
}

BuiltinType BuiltinType::MakeSigned(BuiltinType const& that) noexcept {
    sona_assert(that.IsUnsigned());
    return BuiltinType(TypeId((std::int8_t)that.GetTypeId()
                              + (std::int8_t)TypeId::TI_i8
                              - (std::int8_t)TypeId::TI_u8));
}

BuiltinType BuiltinType::MakeUnsigned(BuiltinType const& that) noexcept {
    sona_assert(that.IsSigned());
    return BuiltinType(TypeId((std::int8_t)that.GetTypeId()
                              - (std::int8_t)TypeId::TI_i8
                              + (std::int8_t)TypeId::TI_u8));
}

/// @todo replace hash functions in the future

size_t BuiltinType::GetHash() const noexcept {
    return Type::GetHash();
}

size_t TupleType::GetHash() const noexcept {
    auto rng =
        sona::linq::from_container(m_ElemTypes).
            transform([](sona::owner<Type> const& t) { return t.borrow(); }).
            transform([](sona::ref_ptr<Type const> t) {
                return t.get().GetHash();
            });

    return std::accumulate(rng.begin(), rng.end(), 0);
}

size_t ArrayType::GetHash() const noexcept {
    return m_Base.borrow().get().GetHash() * GetSize()* 19937;
}

size_t PointerType::GetHash() const noexcept {
    return m_Pointee.borrow().get().GetHash() * (9);
}

size_t FunctionType::GetHash() const noexcept {
    auto rng =
        sona::linq::from_container(m_ParamTypes).
            transform([](sona::owner<Type> const& t) { return t.borrow(); }).
            transform([](sona::ref_ptr<Type const> t) {
                return t.get().GetHash();
            });
    return std::accumulate(rng.begin(), rng.end(), 0)
           + m_ReturnType.borrow().get().GetHash();
}

} // namespace ckx
