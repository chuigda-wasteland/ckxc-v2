#include "AST/Type.h"
#include "AST/Decl.h"

#include "Frontend/Token.h"

#include "sona/util.h"

#include <algorithm>
#include <numeric>
#include <type_traits>

namespace ckx {
namespace AST {

using std::size_t;
template <typename T> using DefaultHash = std::hash<T>;

bool Type::IsBuiltin() const noexcept {
  return GetTypeId() == TypeId::TI_Builtin;
}

bool Type::IsPointer() const noexcept {
  return GetTypeId() == TypeId::TI_Pointer;
}

bool Type::IsReference() const noexcept {
  return GetTypeId() == TypeId::TI_Ref;
}

size_t Type::GetHash() const noexcept {
  using NumericTypeId = std::underlying_type_t<TypeId>;
  return DefaultHash<NumericTypeId>()(static_cast<NumericTypeId>(GetTypeId()));
}

const char*
BuiltinType::GetTypeName(BuiltinType::BuiltinTypeId btid) noexcept {
  switch (btid) {
  #define BUILTIN_TYPE(name, size, isint, \
                     issigned, signedver, unsignedver, token) \
  case BuiltinType::BTI_##name: \
    return #name;
  #include "Syntax/BuiltinTypes.def"
  }
}

bool BuiltinType::IsNumeric(BuiltinType::BuiltinTypeId btid) noexcept {
  return (btid >= BuiltinTypeId::BTI_Int8) &&
         (btid <= BuiltinTypeId::BTI_Quad);
}

bool BuiltinType::IsIntegral(BuiltinType::BuiltinTypeId btid) noexcept {
  switch (btid) {
  #define BUILTIN_TYPE(name, size, isint, \
                       issigned, signedver, unsignedver, token) \
  case BuiltinType::BTI_##name: return isint;
  #include "Syntax/BuiltinTypes.def"
  }
}

bool BuiltinType::IsFloating(BuiltinType::BuiltinTypeId btid) noexcept {
  return btid == BuiltinTypeId::BTI_Float
         || btid == BuiltinTypeId::BTI_Double
         || btid == BuiltinTypeId::BTI_Quad;
}

bool BuiltinType::IsSigned(BuiltinType::BuiltinTypeId btid) noexcept {
  switch (btid) {
  #define BUILTIN_TYPE(name, size, isint, \
                       issigned, signedver, unsignedver, token) \
  case BuiltinType::BTI_##name: return issigned;
  #include "Syntax/BuiltinTypes.def"
  }
}

bool BuiltinType::IsUnsigned(BuiltinType::BuiltinTypeId btid) noexcept {
  switch (btid) {
  #define BUILTIN_TYPE(name, size, isint, \
                       issigned, signedver, unsignedver, token) \
  case BuiltinType::BTI_##name: \
    return (!issigned \
            && BuiltinTypeId::BTI_##signedver != BuiltinTypeId::BTI_NoType);
  #include "Syntax/BuiltinTypes.def"
  }
}

int8_t BuiltinType::SIntRank(BuiltinType::BuiltinTypeId btid) noexcept {
  sona_assert (IsIntegral(btid) && IsSigned(btid));
  switch (btid) {
  #define BUILTIN_TYPE(name, size, isint, \
                       issigned, signedver, unsignedver, token) \
  case BuiltinTypeId::BTI_##name: return size;
  #include "Syntax/BuiltinTypes.def"
  }
}

int8_t BuiltinType::UIntRank(BuiltinType::BuiltinTypeId btid) noexcept {
  sona_assert (IsIntegral(btid) && IsUnsigned(btid));
  switch (btid) {
  #define BUILTIN_TYPE(name, size, isint, \
                       issigned, signedver, unsignedver, token) \
  case BuiltinTypeId::BTI_##name: return size;
  #include "Syntax/BuiltinTypes.def"
  }
}

int8_t BuiltinType::FloatRank(BuiltinType::BuiltinTypeId btid) noexcept  {
  sona_assert (IsIntegral(btid) && IsFloating(btid));
  switch (btid) {
  #define BUILTIN_TYPE(name, size, isint, \
                       issigned, signedver, unsignedver, token) \
  case BuiltinTypeId::BTI_##name: return size;
  #include "Syntax/BuiltinTypes.def"
  }
}

char const *BuiltinType::GetTypeName() const noexcept {
  return GetTypeName(GetBtid());
}

bool BuiltinType::IsNumeric() const noexcept {
  return IsNumeric(GetBtid());
}

bool BuiltinType::IsIntegral() const noexcept {
  return IsIntegral(GetBtid());
}

bool BuiltinType::IsFloating() const noexcept {
  return IsFloating(GetBtid());
}

bool BuiltinType::IsSigned() const noexcept {
  return IsSigned(GetBtid());
}

bool BuiltinType::IsUnsigned() const noexcept {
  return IsUnsigned(GetBtid());
}

/// @todo replace hash functions in the future
size_t BuiltinType::GetHash() const noexcept {
  using NumericBuiltinTypeId = std::underlying_type_t<BuiltinTypeId>;
  using NBTI = NumericBuiltinTypeId;
  return DefaultHash<NBTI>()(static_cast<NBTI>(GetBtid()));
}

bool BuiltinType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Builtin) {
    BuiltinType const &t = static_cast<BuiltinType const &>(that);
    return (GetBtid() == t.GetBtid());
  }
  return false;
}

size_t TupleType::GetHash() const noexcept {
  auto rng = sona::linq::from_container(m_ElemTypes)
             .transform([](QualType t) {
                return t.GetUnqualTy()->GetHash() + t.GetCVR() * 42;
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
          [](std::pair<QualType, QualType> p)
          { return p.first.GetUnqualTy()->EqualTo(p.second.GetUnqualTy().get())
                   && p.first.GetCVR() == p.second.GetCVR(); });

      for (bool b : rng)
        if (!b) return false;
      return true;
    }
  }
  return false;
}

size_t ArrayType::GetHash() const noexcept {
  return m_Base.GetUnqualTy().get().GetHash() * GetSize() * 19937
         + m_Base.GetCVR() * 42;
}

bool ArrayType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Array) {
    ArrayType const &t = static_cast<ArrayType const &>(that);
    return GetBase().GetUnqualTy()->EqualTo(t.GetBase().GetUnqualTy().get())
           && GetBase().GetCVR() == t.GetBase().GetCVR();
  }
  return false;
}

size_t PointerType::GetHash() const noexcept {
  return m_Pointee.GetUnqualTy().get().GetHash() * (9)
         + m_Pointee.GetCVR() * 42;
}

bool PointerType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Pointer) {
    PointerType const &t = static_cast<PointerType const &>(that);
    return GetPointee().GetUnqualTy()->EqualTo(
             t.GetPointee().GetUnqualTy().get())
           && GetPointee().GetCVR() == t.GetPointee().GetCVR();
  }
  return false;
}

size_t FunctionType::GetHash() const noexcept {
  auto rng =
      sona::linq::from_container(m_ParamTypes)
          .transform([](QualType t) {
             return t.GetUnqualTy()->GetHash() + t.GetCVR() * 42; });
  return (size_t)std::accumulate(rng.begin(), rng.end(), 0) +
         m_ReturnType.GetUnqualTy()->GetHash();
}

bool FunctionType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Function) {
    FunctionType const &t = static_cast<FunctionType const &>(that);
    if (GetReturnType() == t.GetReturnType()) {
      auto rng1 = sona::linq::from_container(GetParamTypes());
      auto rng2 = sona::linq::from_container(t.GetParamTypes());
      auto rng = rng1.zip_with(rng2).transform(
          [](std::pair<QualType, QualType> p)
          { return
                 p.first == p.second;
          });
      for (bool b : rng)
        if (!b) return false;
      return true;
    }
  }
  return false;
}

/// @todo remove magic numbers, use better hash algorithms
std::size_t LValueRefType::GetHash() const noexcept {
  return GetReferencedType().GetUnqualTy().get().GetHash() * 19260817
         + GetReferencedType().GetCVR() * 42;
}

std::size_t RValueRefType::GetHash() const noexcept {
  return GetReferencedType().GetUnqualTy().get().GetHash() * 19660813
         + GetReferencedType().GetCVR() * 42;
}

bool RefType::EqualTo(Type const &that) const noexcept {
  if (that.GetTypeId() == TypeId::TI_Ref) {
    RefType const &t = static_cast<RefType const &>(that);
    return (GetRefTypeId() == t.GetRefTypeId())
           && (GetReferencedType().GetUnqualTy()->EqualTo(
                t.GetReferencedType().GetUnqualTy().get()))
           && (GetReferencedType().GetCVR() == t.GetReferencedType().GetCVR());
  }
  return false;
}

bool LValueRefType::EqualTo(Type const &that) const noexcept {
  return RefType::EqualTo(that);
}

bool RValueRefType::EqualTo(Type const &that) const noexcept {
  return RefType::EqualTo(that);
}

ClassType::ClassType(sona::ref_ptr<ClassDecl> decl)
  : UserDefinedType(UDTypeId::UTI_Class, decl->GetName(),
                    decl.cast_unsafe<TypeDecl>()) {}

sona::ref_ptr<const ClassDecl> ClassType::GetClassDecl() const noexcept {
  return GetTypeDecl().cast_unsafe<ClassDecl const>();
}

EnumType::EnumType(sona::ref_ptr<EnumDecl> decl)
  : UserDefinedType(UDTypeId::UTI_Enum, decl->GetName(),
                    decl.cast_unsafe<TypeDecl>()) {}

sona::ref_ptr<const EnumDecl> EnumType::GetEnumDecl() const noexcept {
  return GetTypeDecl().cast_unsafe<EnumDecl const>();
}

ADTType::ADTType(sona::ref_ptr<ADTDecl> decl)
  : UserDefinedType(UDTypeId::UTI_ADT, decl->GetName(),
                    decl.cast_unsafe<TypeDecl>()) {}

sona::ref_ptr<const ADTDecl>
ADTType::GetADTDecl() const noexcept {
  return GetTypeDecl().cast_unsafe<ADTDecl const>();
}

UsingType::UsingType(sona::ref_ptr<UsingDecl> usingDecl)
  : UserDefinedType(UDTypeId::UTI_Using, usingDecl->GetName(),
                    usingDecl.cast_unsafe<AST::TypeDecl>()) {}

sona::ref_ptr<const UsingDecl> UsingType::GetUsingDecl() const noexcept {
  return GetTypeDecl().cast_unsafe<AST::UsingDecl const>();
}

UserDefinedType::UserDefinedType(UserDefinedType::UDTypeId id,
                                 const sona::strhdl_t& typeName,
                                 sona::ref_ptr<TypeDecl> typeDecl)
  : Type(TypeId::TI_UserDefined), m_Id(id),
    m_TypeName(typeName), m_TypeDecl(typeDecl) {
  typeDecl->SetTypeForDecl(this);
}

std::size_t UserDefinedType::GetHash() const noexcept {
  return std::hash<TypeDecl const*>()(&GetTypeDecl().get());
}

bool UserDefinedType::EqualTo(const Type& that) const noexcept {
  if (that.GetTypeId() == Type::TypeId::TI_UserDefined) {
    UserDefinedType const& uthat = static_cast<UserDefinedType const&>(that);
    return uthat.GetUserDefinedTypeId() == this->GetUserDefinedTypeId()
           && uthat.GetTypeDecl() == this->GetTypeDecl();
  }
  return false;
}

sona::ref_ptr<Decl const>
GetDeclOfUserDefinedType(sona::ref_ptr<Type const> ty) noexcept {
  switch (ty->GetTypeId()) {
  case Type::TypeId::TI_UserDefined: {
    sona::ref_ptr<UserDefinedType const> udTy
        = ty.cast_unsafe<UserDefinedType const>();
    return udTy->GetTypeDecl().cast_unsafe<Decl const>();
  }

  default:
    sona_unreachable1("not user defined type");
    return nullptr;
  }
}

sona::owner<Backend::ActionResult>
BuiltinType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitBuiltinType(this);
}

sona::owner<Backend::ActionResult>
TupleType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitTupleType(this);
}

sona::owner<Backend::ActionResult>
ArrayType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitArrayType(this);
}

sona::owner<Backend::ActionResult>
PointerType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitPointerType(this);
}

sona::owner<Backend::ActionResult>
LValueRefType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitLValueRefType(this);
}

sona::owner<Backend::ActionResult>
RValueRefType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitRValueRefType(this);
}

sona::owner<Backend::ActionResult>
FunctionType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitFunctionType(this);
}

sona::owner<Backend::ActionResult>
ClassType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitClassType(this);
}

sona::owner<Backend::ActionResult>
EnumType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitEnumType(this);
}

sona::owner<Backend::ActionResult>
ADTType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitADTType(this);
}

sona::owner<Backend::ActionResult>
UsingType::Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const {
  return visitor->VisitUsingType(this);
}

} // namespace AST
} // namespace ckx
