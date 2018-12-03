#include "AST/Type.hpp"
#include "sona/util.hpp"

#include <algorithm>
#include <numeric>
#include <type_traits>

namespace ckx {
namespace AST {

using std::size_t;
template <typename T> using DefaultHash = std::hash<T>;

size_t Type::GetHash() const noexcept {
  using NumericTypeId = std::underlying_type_t<TypeId>;
  return DefaultHash<NumericTypeId>()(static_cast<NumericTypeId>(GetTypeId()));
}

char const *BuiltinType::GetTypeName() const noexcept {
  switch (GetBuiltinTypeId()) {
  case BuiltinTypeId::BTI_u8: return "vu8";
  case BuiltinTypeId::BTI_u16: return "vu16";
  case BuiltinTypeId::BTI_u32: return "vu32";
  case BuiltinTypeId::BTI_u64: return "vu64";
  case BuiltinTypeId::BTI_i8: return "vi8";
  case BuiltinTypeId::BTI_i16: return "vi16";
  case BuiltinTypeId::BTI_i32: return "vi32";
  case BuiltinTypeId::BTI_i64: return "vi64";
  case BuiltinTypeId::BTI_bool: return "bool";
  case BuiltinTypeId::BTI_nil: return "nil";
  case BuiltinTypeId::BTI_void: return "void";

  default: sona_unreachable();
  }
  return "";
}

bool BuiltinType::IsNumeric() const noexcept {
  return (GetBuiltinTypeId() >= BuiltinTypeId::BTI_u8) &&
         (GetBuiltinTypeId() <= BuiltinTypeId::BTI_r64);
}

bool BuiltinType::IsIntegral() const noexcept {
  return (GetBuiltinTypeId() >= BuiltinTypeId::BTI_u8) &&
         (GetBuiltinTypeId() <= BuiltinTypeId::BTI_i64);
}

bool BuiltinType::IsSigned() const noexcept {
  return IsIntegral() && GetBuiltinTypeId() >= BuiltinTypeId::BTI_i8;
}

bool BuiltinType::IsUnsigned() const noexcept {
  return IsIntegral() && !(IsSigned());
}

BuiltinType BuiltinType::MakeSigned(BuiltinType const &that) noexcept {
  sona_assert(that.IsUnsigned());
  return BuiltinType(
      BuiltinTypeId((NumericBuiltinTypeId)that.GetTypeId() +
                    (NumericBuiltinTypeId)BuiltinTypeId::BTI_i8 -
                    (NumericBuiltinTypeId)BuiltinTypeId::BTI_u8));
}

BuiltinType BuiltinType::MakeUnsigned(BuiltinType const &that) noexcept {
  sona_assert(that.IsSigned());
  return BuiltinType(
      BuiltinTypeId((NumericBuiltinTypeId)that.GetTypeId() -
                    (NumericBuiltinTypeId)BuiltinTypeId::BTI_i8 +
                    (NumericBuiltinTypeId)BuiltinTypeId::BTI_u8));
}

/// @todo replace hash functions in the future

size_t BuiltinType::GetHash() const noexcept {
  using NumericBuiltinTypeId = std::underlying_type_t<BuiltinTypeId>;
  using NBTI = NumericBuiltinTypeId;
  return DefaultHash<NBTI>()(static_cast<NBTI>(GetBuiltinTypeId()));
}

bool BuiltinType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Builtin) {
    BuiltinType const &t = static_cast<BuiltinType const &>(that);
    return (GetBuiltinTypeId() == t.GetBuiltinTypeId());
  }
  return false;
}

size_t TupleType::GetHash() const noexcept {
  auto rng = sona::linq::from_container(m_ElemTypes)
             .transform([](sona::ref_ptr<Type const> t) {
                return t.get().GetHash();
              });

  return (size_t)std::accumulate(rng.begin(), rng.end(), 0);
}

bool TupleType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Tuple) {
    TupleType const &t = static_cast<TupleType const &>(that);
    if (GetTupleElemTypes().size() == t.GetTupleElemTypes().size()) {
      auto rng1 = sona::linq::from_container(GetTupleElemTypes());
      auto rng2 = sona::linq::from_container(t.GetTupleElemTypes());
      auto rng = rng1.zip_with(rng2).transform(
          [](std::pair<sona::ref_ptr<Type>, sona::ref_ptr<Type>> p)
          { return p.first.get().EqualTo(p.second.get()); });

      for (bool b : rng)
        if (!b) return false;
      return true;
    }
  }
  return false;
}

size_t ArrayType::GetHash() const noexcept {
  return m_Base.get().GetHash() * GetSize() * 19937;
}

bool ArrayType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Array) {
    ArrayType const &t = static_cast<ArrayType const &>(that);
    return GetBase().get().EqualTo(t.GetBase());
  }
  return false;
}

size_t PointerType::GetHash() const noexcept {
  return m_Pointee.get().GetHash() * (9);
}

bool PointerType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Pointer) {
    PointerType const &t = static_cast<PointerType const &>(that);
    return t.GetPointee().get().EqualTo(t.GetPointee());
  }
  return false;
}

size_t FunctionType::GetHash() const noexcept {
  auto rng =
      sona::linq::from_container(m_ParamTypes)
          .transform([](sona::owner<Type> const &t) { return t.borrow(); })
          .transform(
              [](sona::ref_ptr<Type const> t) { return t.get().GetHash(); });
  return (size_t)std::accumulate(rng.begin(), rng.end(), 0) +
         m_ReturnType.borrow().get().GetHash();
}

bool FunctionType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Function) {
    FunctionType const &t = static_cast<FunctionType const &>(that);
    if (GetReturnType().get().EqualTo(t.GetReturnType()) &&
        GetParamTypes().size() == t.GetParamTypes().size()) {
      auto rng1 =
          sona::linq::from_container(GetParamTypes())
              .transform([](sona::owner<Type> const &t) { return t.borrow(); });

      auto rng2 =
          sona::linq::from_container(t.GetParamTypes())
              .transform([](sona::owner<Type> const &t) { return t.borrow(); });

      auto rng = rng1.zip_with(rng2).transform(
          [](std::pair<sona::ref_ptr<Type const>, sona::ref_ptr<Type const>>
                 p) { return p.first.get().EqualTo(p.second.get()); });

      for (bool b : rng)
        if (!b) return false;
      return true;
    }
  }
  return false;
}

/// @todo remove magic numbers

std::size_t LValueRefType::GetHash() const noexcept {
  return GetReferencedType().get().GetHash() * 19260817;
}

std::size_t RValueRefType::GetHash() const noexcept {
  return GetReferencedType().get().GetHash() * 19660813;
}

std::size_t UsingType::GetHash() const noexcept {
  return m_Aliasee.get().GetHash() * 233;
}

bool RefType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Ref) {
    RefType const &t = static_cast<RefType const &>(that);
    return (GetRefTypeId() == t.GetRefTypeId()) &&
           (GetReferencedType().get().EqualTo(t.GetReferencedType()));
  }
  return false;
}

bool LValueRefType::EqualTo(Type const &that) const noexcept {
  return RefType::EqualTo(that);
}

bool RValueRefType::EqualTo(Type const &that) const noexcept {
  return RefType::EqualTo(that);
}

bool UsingType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Using) {
    UsingType const &t = static_cast<UsingType const &>(that);
    return GetAliasee().get().EqualTo(t);
  }
  return false;
}

} // namespace AST
} // namespace ckx
