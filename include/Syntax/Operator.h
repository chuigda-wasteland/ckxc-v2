#ifndef SYNTAX_OPERATOR_H
#define SYNTAX_OPERATOR_H

#include <cstdint>
#include <Frontend/Token.h>

namespace ckx {
namespace Syntax {

enum class UnaryOperator {
#define UNARY_OP_DEF(name, rep, text) UOP_##name,
#include "Syntax/Operators.def"
  UOP_Invalid
};

enum class BinaryOperator {
#define BINARY_OP_DEF(name, rep, text) BOP_##name,
#include "Syntax/Operators.def"
  BOP_Invalid
};

enum class AssignOperator {
#define ASSIGN_OP_DEF(name, rep, text) AOP_##name,
#include "Syntax/Operators.def"
  AOP_Invalid
};

/// @todo I'm not sure whether this will be used in further days
enum class MixFixOperator {
  MOP_Conditional
};

/// @todo Is there a better way of implementing Cast "operator"s?
enum class CastOperator {
  COP_StaticCast, COP_ConstCast, COP_BitCast
};

std::uint16_t PrecOf(Syntax::BinaryOperator bop) noexcept;

} // namespace Syntax
} // namespace ckx

#endif // OPERATOR_H
