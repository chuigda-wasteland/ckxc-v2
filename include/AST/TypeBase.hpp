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
        ty_u8,
        ty_u16,
        ty_u32,
        ty_u64,
        ty_i8,
        ty_i16,
        ty_i32,
        ty_i64,
        ty_r32,
        ty_r64,
        /// @todo I don't know how to implement char elegantly
            // ty_char,
        ty_bool,
        ty_nil,
        ty_void,

        /// Composed
        ty_tuple,
        ty_array,
        ty_pointer,
        ty_function,

        /// User-defined
        ty_enum,
        ty_class,
        ty_using
    };

    TypeId GetTypeId() const { return m_Id; }

protected:
    Type(TypeId id) : m_Id(id) {}

private:
    TypeId m_Id;
};

} // namespace ckx

#endif // TYPEBASE_HPP
