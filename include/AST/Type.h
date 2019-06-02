#ifndef TYPE_HPP
#define TYPE_HPP

#include "DeclBase.hpp"
#include "TypeBase.hpp"
#include "sona/linq.hpp"
#include "sona/pointer_plus.hpp"
#include "sona/range.hpp"
#include "sona/small_vector.hpp"

#include <memory>
#include <type_traits>
#include <vector>

namespace ckx {
namespace AST {

class BuiltinType final : public Type {
public:
  enum class BuiltinTypeId : std::int8_t {
    BTI_u8,
    BTI_u16,
    BTI_u32,
    BTI_u64,
    BTI_i8,
    BTI_i16,
    BTI_i32,
    BTI_i64,
    BTI_r32,
    BTI_r64,
    BTI_r128,
    BTI_bool,
    BTI_nil,
    BTI_void,
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

class TupleType final : public Type {
public:
  using TupleElements_t = std::vector<sona::ref_ptr<Type>>;

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
  ArrayType(sona::ref_ptr<Type> base, std::size_t size)
      : Type(TypeId::TI_Array), m_Base(base), m_Size(size) {}

  sona::ref_ptr<Type const> GetBase() const { return m_Base; }
  std::size_t GetSize() const { return m_Size; }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;

private:
  sona::ref_ptr<Type> m_Base;
  std::size_t m_Size;
};

class PointerType final : public Type {
public:
  PointerType(sona::ref_ptr<Type const> pointee)
      : Type(TypeId::TI_Pointer), m_Pointee(pointee) {}

  sona::ref_ptr<Type const> GetPointee() const { return m_Pointee; }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;

private:
  sona::ref_ptr<Type const> m_Pointee;
};

class RefType : public Type {
public:
  enum class RefTypeId { RTI_LValueRef, RTI_RValueRef };
  RefType(RefTypeId refTypeId, sona::ref_ptr<Type const> referenced)
      : Type(TypeId::TI_Ref), m_RefTypeId(refTypeId),
        m_ReferencedType(referenced) {}

  RefTypeId GetRefTypeId() const noexcept { return m_RefTypeId; }

  sona::ref_ptr<Type const> GetReferencedType() const noexcept {
    return m_ReferencedType;
  }

  std::size_t GetHash() const noexcept override = 0;
  bool EqualTo(Type const &that) const noexcept override = 0;

private:
  RefTypeId m_RefTypeId;
  sona::ref_ptr<Type const> m_ReferencedType;
};

class LValueRefType final : public RefType {
public:
  LValueRefType(sona::ref_ptr<Type const> referenced)
      : RefType(RefTypeId::RTI_LValueRef, referenced) {}

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;
};

class RValueRefType final : public RefType {
public:
  RValueRefType(sona::ref_ptr<Type const> referenced)
      : RefType(RefTypeId::RTI_RValueRef, referenced) {}

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;
};

class FunctionType final : public Type {
public:
  FunctionType(std::vector<sona::ref_ptr<Type>> &&paramTypes,
               sona::ref_ptr<Type> returnType)
      : Type(TypeId::TI_Function), m_ParamTypes(std::move(paramTypes)),
        m_ReturnType(std::move(returnType)) {}

  std::vector<sona::ref_ptr<Type>> const& GetParamTypes() const {
    return m_ParamTypes;
  }

  sona::ref_ptr<Type const> GetReturnType() const {
    return m_ReturnType;
  }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const override;

private:
  std::vector<sona::ref_ptr<Type>> m_ParamTypes;
  sona::ref_ptr<Type> m_ReturnType;
};

class UserDefinedType : public Type {
public:
  enum class UDTypeId { UTI_Class, UTI_Enum, UTI_EnumClass, UTI_Using };
  UserDefinedType(UDTypeId id, sona::string_ref const& typeName,
                  sona::ref_ptr<TypeDecl> typeDecl);

  UDTypeId GetUserDefinedTypeId() const noexcept { return m_Id; }

  sona::string_ref const &GetTypeName() const noexcept { return m_TypeName; }

  std::size_t GetHash() const noexcept override final;
  bool EqualTo(Type const &that) const noexcept override final;

  sona::ref_ptr<AST::TypeDecl const> GetTypeDecl() const noexcept {
    return m_TypeDecl;
  }

private:
  UDTypeId m_Id;
  sona::string_ref m_TypeName;
  sona::ref_ptr<AST::TypeDecl> m_TypeDecl;
};

/// @todo How to calculate hash of class and enum types?
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

class EnumClassType final : public UserDefinedType {
public:
  EnumClassType(sona::ref_ptr<EnumClassDecl> decl);
  sona::ref_ptr<EnumClassDecl const> GetEnumClassDecl() const noexcept;

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

#endif // TYPE_HPP
