#ifndef TYPE_HPP
#define TYPE_HPP

#include "TypeBase.hpp"
#include "sona/pointer_plus.hpp"
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

    std::size_t GetHash() const noexcept override;
};

class TupleType : public Type {
public:
    using TupleElements_t = sona::small_vector<sona::owner<Type>, 3>;

    TupleType(TupleElements_t&& elemTypes)
        : Type(TypeId::TI_tuple),
          m_ElemTypes(std::move(elemTypes)) {}

    /// ??? GetTupleElemTypes() const { return ???; }
    // TupleElements_t const& GetTupleElemTypes() const { return m_ElemTypes; }
    std::size_t GetTupleSize() const { return m_ElemTypes.size(); }

    std::size_t GetHash() const noexcept override;

private:
    TupleElements_t m_ElemTypes;
};

class ArrayType : public Type {
public:
    ArrayType(sona::owner<Type> &&base, std::size_t size)
        : Type(TypeId::TI_array),
          m_Base(std::move(base)),
          m_Size(size) {}

    sona::ref_ptr<Type const> GetBase() const { return m_Base.borrow(); }
    std::size_t GetSize() const { return m_Size; }

    std::size_t GetHash() const noexcept override;

private:
    sona::owner<Type> m_Base;
    std::size_t m_Size;
};

class PointerType : public Type {
public:
    PointerType(sona::owner<Type> &&pointee)
        : Type(TypeId::TI_pointer),
          m_Pointee(std::move(pointee)) {}

    sona::ref_ptr<Type const> GetPointee() const { return m_Pointee.borrow(); }

    std::size_t GetHash() const noexcept override;

private:
    sona::owner<Type> m_Pointee;
};

class FunctionType : public Type {
public:
    FunctionType(std::vector<sona::owner<Type>> &&paramTypes,
                 sona::owner<Type> returnType) :
        Type(TypeId::TI_function),
        m_ParamTypes(std::move(paramTypes)),
        m_ReturnType(std::move(returnType)) {}

    // std::vector<Type*> const& GetParamTypes() const { return m_ParamTypes; }
    sona::ref_ptr<Type const> GetReturnType() const {
        return m_ReturnType.borrow();
    }

    std::size_t GetHash() const noexcept override;

private:
    std::vector<sona::owner<Type>> m_ParamTypes;
    sona::owner<Type> m_ReturnType;
};

/// @todo implement class type and enum type after finishing Declaration
class ClassType : public Type {};
class EnumType : public Type {};

} // namespace ckx

#endif // TYPE_HPP
