#ifndef TYPE_H
#define TYPE_H

#include "DeclBase.h"
#include "TypeBase.h"
#include "sona/linq.h"
#include "sona/pointer_plus.h"
#include "sona/range.h"
#include "sona/small_vector.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace ckx {
namespace AST {

class BuiltinType final : public Type {
public:
  enum class BuiltinTypeId : std::int8_t {
    #define BUILTIN_TYPE(name, rep, size, isint, \
                         issigned, signedver, unsignedver) \
      BTI_##name,
    #include "Syntax/BuiltinTypes.def"
  };

  explicit BuiltinType(BuiltinTypeId id)
      : Type(TypeId::TI_Builtin), m_BuiltinTypeId(id) {}

  BuiltinTypeId GetBuiltinTypeId() const noexcept { return m_BuiltinTypeId; }

  char const *GetTypeName() const noexcept;
  bool IsNumeric() const noexcept;
  bool IsIntegral() const noexcept;
  bool IsFloating() const noexcept;
  bool IsSigned() const noexcept;
  bool IsUnsigned() const noexcept;
  static BuiltinType MakeSigned(BuiltinType const &that) noexcept;
  static BuiltinType MakeUnsigned(BuiltinType const &that) noexcept;

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;

private:
  BuiltinTypeId m_BuiltinTypeId;

  using NumericBuiltinTypeId = std::underlying_type_t<BuiltinTypeId>;
};

/// @todo It may be hard to implement tuple with current type system.
class TupleType final : public Type {
public:
  using TupleElements_t = std::vector<QualType>;

  TupleType(TupleElements_t &&elemTypes)
      : Type(TypeId::TI_Tuple), m_ElemTypes(std::move(elemTypes)) {}

  TupleElements_t const &GetTupleElemTypes() const noexcept {
    return m_ElemTypes;
  }

  std::size_t GetTupleSize() const { return m_ElemTypes.size(); }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;

private:
  TupleElements_t m_ElemTypes;
};

class ArrayType final : public Type {
public:
  ArrayType(QualType base, std::size_t size)
      : Type(TypeId::TI_Array), m_Base(base), m_Size(size) {}

  QualType GetBase() const { return m_Base; }
  std::size_t GetSize() const { return m_Size; }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;

private:
  QualType m_Base;
  std::size_t m_Size;
};

class PointerType final : public Type {
public:
  PointerType(QualType pointee)
      : Type(TypeId::TI_Pointer), m_Pointee(pointee) {}

  QualType GetPointee() const { return m_Pointee; }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;

private:
  QualType m_Pointee;
};

class RefType : public Type {
public:
  enum class RefTypeId { RTI_LValueRef, RTI_RValueRef };
  RefType(RefTypeId refTypeId, QualType referenced)
      : Type(TypeId::TI_Ref), m_RefTypeId(refTypeId),
        m_ReferencedType(referenced) {}

  RefTypeId GetRefTypeId() const noexcept { return m_RefTypeId; }

  QualType GetReferencedType() const noexcept {
    return m_ReferencedType;
  }

  std::size_t GetHash() const noexcept override = 0;
  bool EqualTo(Type const &that) const noexcept override = 0;

private:
  RefTypeId m_RefTypeId;
  QualType m_ReferencedType;
};

class LValueRefType final : public RefType {
public:
  LValueRefType(QualType referenced)
      : RefType(RefTypeId::RTI_LValueRef, referenced) {}

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;
};

class RValueRefType final : public RefType {
public:
  RValueRefType(QualType referenced)
      : RefType(RefTypeId::RTI_RValueRef, referenced) {}

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;
};

class FunctionType final : public Type {
public:
  FunctionType(std::vector<QualType> &&paramTypes,
               QualType returnType)
      : Type(TypeId::TI_Function), m_ParamTypes(std::move(paramTypes)),
        m_ReturnType(std::move(returnType)) {}

  std::vector<QualType> const& GetParamTypes() const {
    return m_ParamTypes;
  }

  QualType GetReturnType() const {
    return m_ReturnType;
  }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;

private:
  std::vector<QualType> m_ParamTypes;
  QualType m_ReturnType;
};

class UserDefinedType : public Type {
public:
  enum class UDTypeId { UTI_Class, UTI_Enum, UTI_ADT, UTI_Using };
  UserDefinedType(UDTypeId id, sona::strhdl_t const& typeName,
                  sona::ref_ptr<TypeDecl> typeDecl);

  UDTypeId GetUserDefinedTypeId() const noexcept { return m_Id; }

  sona::strhdl_t const &GetTypeName() const noexcept { return m_TypeName; }

  std::size_t GetHash() const noexcept override final;
  bool EqualTo(Type const &that) const noexcept override final;

  sona::ref_ptr<AST::TypeDecl const> GetTypeDecl() const noexcept {
    return m_TypeDecl;
  }

private:
  UDTypeId m_Id;
  sona::strhdl_t m_TypeName;
  sona::ref_ptr<AST::TypeDecl> m_TypeDecl;
};

class ClassType final : public UserDefinedType {
public:
  ClassType(sona::ref_ptr<ClassDecl> decl);
  sona::ref_ptr<ClassDecl const> GetClassDecl() const noexcept;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;
};

class EnumType final : public UserDefinedType {
public:
  EnumType(sona::ref_ptr<EnumDecl> decl);
  sona::ref_ptr<EnumDecl const> GetEnumDecl() const noexcept;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;
};

class ADTType final : public UserDefinedType {
public:
  ADTType(sona::ref_ptr<ADTDecl> decl);
  sona::ref_ptr<ADTDecl const> GetADTDecl() const noexcept;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;
};

class UsingType final : public UserDefinedType {
public:
  UsingType(sona::ref_ptr<AST::UsingDecl> usingDecl);
  sona::ref_ptr<AST::UsingDecl const> GetUsingDecl() const noexcept;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;
};

sona::ref_ptr<AST::Decl const>
GetDeclOfUserDefinedType(sona::ref_ptr<AST::Type const> ty) noexcept;

} // namespace AST
} // namespace ckx

#endif // TYPE_H
