#ifndef TYPE_HPP
#define TYPE_HPP

#include "TypeBase.hpp"
#include "DeclBase.hpp"
#include "sona/pointer_plus.hpp"
#include "sona/small_vector.hpp"
#include "sona/range.hpp"
#include "sona/linq.hpp"

#include <vector>
#include <memory>
#include <type_traits>

namespace ckx {

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
        /// @todo I don't know how to implement char elegantly
            // TI_char,
        BTI_bool,
        BTI_nil,
        BTI_void,
    };

    BuiltinType(BuiltinTypeId id)
        : Type(TypeId::TI_Builtin),
          m_BuiltinTypeId(id) {}

    BuiltinTypeId GetBuiltinTypeId() const noexcept {
        return m_BuiltinTypeId;
    }

    char const* GetTypeName() const noexcept;
    bool IsNumeric() const noexcept;
    bool IsIntegral() const noexcept;
    bool IsFloating() const noexcept;
    bool IsSigned() const noexcept;
    bool IsUnsigned() const noexcept;
    static BuiltinType MakeSigned(BuiltinType const& that) noexcept;
    static BuiltinType MakeUnsigned(BuiltinType const& that) noexcept;

    std::size_t GetHash() const noexcept override;
    bool EqualTo(Type const& that) const noexcept override;

private:
    BuiltinTypeId m_BuiltinTypeId;

    using NumericBuiltinTypeId = std::underlying_type_t<BuiltinTypeId>;
};

class TupleType : public Type {
public:
    using TupleElements_t = sona::small_vector<sona::owner<Type>, 3>;

    TupleType(TupleElements_t&& elemTypes)
        : Type(TypeId::TI_Tuple),
          m_ElemTypes(std::move(elemTypes)) {}

    TupleElements_t const& GetTupleElemTypes() const noexcept {
        return m_ElemTypes;
    }

    std::size_t GetTupleSize() const { return m_ElemTypes.size(); }

    std::size_t GetHash() const noexcept override;
    bool EqualTo(Type const& that) const noexcept override;

private:
    TupleElements_t m_ElemTypes;
};

class ArrayType : public Type {
public:
    ArrayType(sona::owner<Type> &&base, std::size_t size)
        : Type(TypeId::TI_Array),
          m_Base(std::move(base)),
          m_Size(size) {}

    sona::ref_ptr<Type const> GetBase() const { return m_Base.borrow(); }
    std::size_t GetSize() const { return m_Size; }

    std::size_t GetHash() const noexcept override;
    bool EqualTo(Type const& that) const noexcept override;

private:
    sona::owner<Type> m_Base;
    std::size_t m_Size;
};

class PointerType : public Type {
public:
    PointerType(sona::owner<Type> &&pointee)
        : Type(TypeId::TI_Pointer),
          m_Pointee(std::move(pointee)) {}

    sona::ref_ptr<Type const> GetPointee() const { return m_Pointee.borrow(); }

    std::size_t GetHash() const noexcept override;
    bool EqualTo(Type const& that) const noexcept override;

private:
    sona::owner<Type> m_Pointee;
};

class RefType : public Type {
public:
    enum class RefTypeId { RTI_LValueRef, RTI_RValueRef };
    RefType(RefTypeId refTypeId,
            sona::owner<Type> &&referenced)
        : Type(TypeId::TI_Ref),
          m_RefTypeId(refTypeId),
          m_ReferencedType(std::move(referenced)) {}

    RefTypeId GetRefTypeId() const noexcept {
        return m_RefTypeId;
    }

    sona::ref_ptr<Type const> GetReferencedType() const noexcept {
        return m_ReferencedType.borrow();
    }

private:
    RefTypeId m_RefTypeId;
    sona::owner<Type> m_ReferencedType;
};

class LValueRefType : public RefType {
public:
    LValueRefType(sona::owner<Type> &&referenced)
        : RefType(RefTypeId::RTI_LValueRef,
                  std::move(referenced)) {}
};

class RValueRefType : public RefType {
public:
    RValueRefType(sona::owner<Type> &&referenced)
        : RefType(RefTypeId::RTI_RValueRef,
                  std::move(referenced)) {}
};

class FunctionType : public Type {
public:
    FunctionType(std::vector<sona::owner<Type>> &&paramTypes,
                 sona::owner<Type> returnType) :
        Type(TypeId::TI_Function),
        m_ParamTypes(std::move(paramTypes)),
        m_ReturnType(std::move(returnType)) {}

    std::vector<sona::owner<Type>> const& GetParamTypes() const {
        return m_ParamTypes;
    }

    sona::ref_ptr<Type const> GetReturnType() const {
        return m_ReturnType.borrow();
    }

    std::size_t GetHash() const noexcept override;
    bool EqualTo(Type const& that) const noexcept override;

private:
    std::vector<sona::owner<Type>> m_ParamTypes;
    sona::owner<Type> m_ReturnType;
};

class TagType : public Type {
public:
    enum class TagTypeId { TTI_Class, TTI_Enum };
    TagType(TagTypeId id,
            std::string &&typeName,
            SourceRange &&typeNameRange,
            SourceLocation leftBraceLocation,
            SourceLocation rightBraceLocation)
        : Type(TypeId::TI_Tag),
          m_Id(id),
          m_TypeName(std::move(typeName)),
          m_TypeNameRange(std::move(typeNameRange)),
          m_LeftBraceLocation(leftBraceLocation),
          m_RightBraceLocation(rightBraceLocation) {}

    TagTypeId GetTagTypeId() const noexcept {
        return m_Id;
    }

    std::string const& GetTypeName() const noexcept {
        return m_TypeName;
    }

    SourceRange const& GetTypeNameRange() const noexcept {
        return m_TypeNameRange;
    }

    SourceLocation GetLeftBraceLocation() const noexcept {
        return m_LeftBraceLocation;
    }

    SourceLocation GetRightBraceLocation() const noexcept {
        return m_RightBraceLocation;
    }

    std::size_t GetHash() const noexcept override = 0;
    bool EqualTo(Type const& that) const noexcept override = 0;

private:
    TagTypeId m_Id;
    std::string m_TypeName;
    SourceRange m_TypeNameRange;
    SourceLocation m_LeftBraceLocation,
                   m_RightBraceLocation;
};

/// @todo How to calculate hash of class and enum types?
class ClassType : public TagType {
public:
    ClassType(std::string &&typeName,
              SourceRange &&typeNameRange,
              sona::ref_ptr<ClassDecl> decl,
              SourceLocation leftBraceLocation,
              SourceLocation rightBraceLocation)
        : TagType(TagTypeId::TTI_Class,
                  std::move(typeName), std::move(typeNameRange),
                  leftBraceLocation, rightBraceLocation),
          m_Decl(decl) {}

    sona::ref_ptr<ClassDecl const> GetDecl() const noexcept {
        return m_Decl;
    }

    std::size_t GetHash() const noexcept override;
    bool EqualTo(Type const& that) const noexcept override;

private:
    sona::ref_ptr<ClassDecl> m_Decl;
};

class EnumType : public TagType {
public:
    EnumType(std::string &&typeName,
             SourceRange &&typeNameRange,
             sona::ref_ptr<EnumDecl> decl,
             SourceLocation leftBraceLocation,
             SourceLocation rightBraceLocation)
        : TagType(TagTypeId::TTI_Enum,
                  std::move(typeName), std::move(typeNameRange),
                  leftBraceLocation, rightBraceLocation),
          m_Decl(decl) {}

    sona::ref_ptr<EnumDecl const> GetDecl() const noexcept {
        return m_Decl;
    }

    std::size_t GetHash() const noexcept override;
    bool EqualTo(Type const& that) const noexcept override;

private:
    sona::ref_ptr<EnumDecl> m_Decl;
};

class UsingType : public Type {
public:
    UsingType(sona::ref_ptr<Type> aliasee)
        : Type(TypeId::TI_Using),
          m_Aliasee(aliasee) {}

    sona::ref_ptr<Type const> GetAliasee() const noexcept {
        return m_Aliasee;
    }

private:
    sona::ref_ptr<Type> m_Aliasee;
};

} // namespace ckx

#endif // TYPE_HPP
