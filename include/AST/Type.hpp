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

class BuiltinType : public Type {
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
    /// @todo I don't know how to implement char elegantly
    /// TI_char,
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

private:
  BuiltinTypeId m_BuiltinTypeId;

  using NumericBuiltinTypeId = std::underlying_type_t<BuiltinTypeId>;
};

class TupleType : public Type {
public:
  using TupleElements_t = sona::small_vector<sona::ref_ptr<Type>, 3>;

  TupleType(TupleElements_t &&elemTypes)
      : Type(TypeId::TI_Tuple), m_ElemTypes(std::move(elemTypes)) {}

  TupleElements_t const &GetTupleElemTypes() const noexcept {
    return m_ElemTypes;
  }

  std::size_t GetTupleSize() const { return m_ElemTypes.size(); }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

private:
  TupleElements_t m_ElemTypes;
};

class ArrayType : public Type {
public:
  ArrayType(sona::ref_ptr<Type> base, std::size_t size)
      : Type(TypeId::TI_Array), m_Base(base), m_Size(size) {}

  sona::ref_ptr<Type const> GetBase() const { return m_Base; }
  std::size_t GetSize() const { return m_Size; }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

private:
  sona::ref_ptr<Type> m_Base;
  std::size_t m_Size;
};

class PointerType : public Type {
public:
  PointerType(sona::ref_ptr<Type const> pointee)
      : Type(TypeId::TI_Pointer), m_Pointee(pointee) {}

  sona::ref_ptr<Type const> GetPointee() const { return m_Pointee; }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

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

class LValueRefType : public RefType {
public:
  LValueRefType(sona::ref_ptr<Type const> referenced)
      : RefType(RefTypeId::RTI_LValueRef, referenced) {}

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;
};

class RValueRefType : public RefType {
public:
  RValueRefType(sona::ref_ptr<Type const> referenced)
      : RefType(RefTypeId::RTI_RValueRef, referenced) {}

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;
};

class FunctionType : public Type {
public:
  FunctionType(std::vector<sona::owner<Type>> &&paramTypes,
               sona::owner<Type> returnType)
      : Type(TypeId::TI_Function), m_ParamTypes(std::move(paramTypes)),
        m_ReturnType(std::move(returnType)) {}

  std::vector<sona::owner<Type>> const &GetParamTypes() const {
    return m_ParamTypes;
  }

  sona::ref_ptr<Type const> GetReturnType() const {
    return m_ReturnType.borrow();
  }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

private:
  std::vector<sona::owner<Type>> m_ParamTypes;
  sona::owner<Type> m_ReturnType;
};

class TagType : public Type {
public:
  enum class TagTypeId { TTI_Class, TTI_Enum };
  TagType(TagTypeId id, sona::string_ref const& typeName)
    : Type(TypeId::TI_Tag), m_Id(id), m_TypeName(typeName) {}

  TagTypeId GetTagTypeId() const noexcept { return m_Id; }

  sona::string_ref const &GetTypeName() const noexcept { return m_TypeName; }

  std::size_t GetHash() const noexcept override = 0;
  bool EqualTo(Type const &that) const noexcept override = 0;

private:
  TagTypeId m_Id;
  sona::string_ref m_TypeName;
};

/// @todo How to calculate hash of class and enum types?
class ClassType : public TagType {
public:
  ClassType(sona::string_ref const& typeName , sona::ref_ptr<ClassDecl> decl)
      : TagType(TagTypeId::TTI_Class, typeName), m_Decl(decl) {}

  sona::ref_ptr<ClassDecl const> GetDecl() const noexcept { return m_Decl; }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

private:
  sona::ref_ptr<ClassDecl> m_Decl;
};

class EnumType : public TagType {
public:
  EnumType(sona::string_ref const& typeName, sona::ref_ptr<EnumDecl> decl)
      : TagType(TagTypeId::TTI_Enum, typeName), m_Decl(decl) {}

  sona::ref_ptr<EnumDecl const> GetDecl() const noexcept { return m_Decl; }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

private:
  sona::ref_ptr<EnumDecl> m_Decl;
};

class UsingType : public Type {
public:
  UsingType(sona::ref_ptr<Type> aliasee)
      : Type(TypeId::TI_Using), m_Aliasee(aliasee) {}

  sona::ref_ptr<Type const> GetAliasee() const noexcept { return m_Aliasee; }

  std::size_t GetHash() const noexcept override;
  bool EqualTo(Type const &that) const noexcept override;

private:
  sona::ref_ptr<Type> m_Aliasee;
};

} // namespace AST
} // namespace ckx

#endif // TYPE_HPP
