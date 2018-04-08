#ifndef TYPE_HPP
#define TYPE_HPP

#include "TypeBase.hpp"
#include "sona/small_vector.hpp"
#include "sona/range.hpp"
#include "sona/linq.hpp"

#include <vector>
#include <memory>

namespace ckx {

class BuiltinType : public Type {
public:
    BuiltinType(TypeId id) : Type(id) {}

    char const* GetTypeName() const noexcept;
    bool IsNumeric() const noexcept;
    bool IsIntegral() const noexcept;
    bool IsFloating() const noexcept;
    bool IsSigned() const noexcept;
    bool IsUnsigned() const noexcept;
    static BuiltinType MakeSigned(BuiltinType const& that) noexcept;
    static BuiltinType MakeUnsigned(BuiltinType const& that) noexcept;
};

class TupleType : public Type {
public:
    using TupleElements_t = sona::small_vector<Type*, 3>;

    TupleType(TupleElements_t&& elemTypes)
        : Type(TypeId::ty_tuple),
          m_ElemTypes(std::move(elemTypes)) {}

    TupleElements_t const& GetTupleElemTypes() const { return m_ElemTypes; }
    std::size_t GetTupleSize() const { return m_ElemTypes.size(); }

private:
    TupleElements_t m_ElemTypes;
};

class ArrayType : public Type {
public:
    ArrayType(Type *base, std::size_t size)
        : Type(TypeId::ty_array), m_Base(base), m_Size(size) {}

    Type* GetBase() const { return m_Base; }
    std::size_t GetSize() const { return m_Size; }

private:
    Type *m_Base;
    std::size_t m_Size;
};

class PointerType : public Type {
public:
    PointerType(Type* pointee) : Type(TypeId::ty_pointer), m_Pointee(pointee) {}

    Type* GetPointee() const { return m_Pointee; }

private:
    Type *m_Pointee;
};

class FunctionType : public Type {
public:
    FunctionType(std::vector<Type*> &&paramTypes, Type *returnType) :
        Type(TypeId::ty_function),
        m_ParamTypes(std::move(paramTypes)),
        m_ReturnType(returnType) {}

    std::vector<Type*> const& GetParamTypes() const { return m_ParamTypes; }
    Type* GetReturnType() const { return m_ReturnType; }

private:
    std::vector<Type*> m_ParamTypes;
    Type *m_ReturnType;
};

/// @todo implement class type and enum type after finishing Declaration
class ClassType : public Type {};
class EnumType : public Type {};

} // namespace ckx

#endif // TYPE_HPP
