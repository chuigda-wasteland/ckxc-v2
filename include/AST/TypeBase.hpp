#ifndef TYPEBASE_HPP
#define TYPEBASE_HPP

#include "Basic/SourceRange.hpp"

#include <cstdint>

namespace ckx {

class Type;
    class BuiltinType;
    class TupleType;
    class ArrayType;
    class PointerType;
    class FunctionType;
    class EnumType;
    class ClassType;
    class UsingType;

class Type {
public:
    enum class TypeId : std::int8_t {
        /// Basic / Builtin
        TI_u8,
        TI_u16,
        TI_u32,
        TI_u64,
        TI_i8,
        TI_i16,
        TI_i32,
        TI_i64,
        TI_r32,
        TI_r64,
        /// @todo I don't know how to implement char elegantly
            // TI_char,
        TI_bool,
        TI_nil,
        TI_void,

        /// Composed
        TI_tuple,
        TI_array,
        TI_pointer,
        TI_function,

        /// User-defined
        TI_enum,
        TI_class,
        TI_using
    };

    TypeId GetTypeId() const { return m_Id; }

protected:
    Type(TypeId id) : m_Id(id) {}

private:
    TypeId m_Id;
};

} // namespace ckx

#endif // TYPEBASE_HPP
