#ifndef OPERATOR_H
#define OPERATOR_H

#include <cstdint>
#include <Frontend/Token.h>

namespace ckx {
namespace Syntax {

enum class UnaryOperator {
  UOP_SelfIncr,
  UOP_SelfDecr,
  UOP_PointerTo,
  UOP_Deref,
  UOP_Positive,
  UOP_Negative,
  UOP_BitReverse, UOP_LogicNot
};

enum class BinaryOperator {
  BOP_Add, BOP_Sub, BOP_Mul, BOP_Div, BOP_Mod,
  BOP_LogicAnd, BOP_LogicOr, BOP_LogicNot, BOP_LogicXor,
  BOP_BitAnd, BOP_BitOr, BOP_BitNot, BOP_BitXor,
  BOP_Lt, BOP_Gt, BOP_Eq, BOP_LEq, BOP_GEq, BOP_NEq,

  BOP_Invalid
};

/// @todo I'm not sure whether this will be used in further days
enum class MixFixOperator {
  MOP_Conditional
};

/// @todo Is there a better way of implementing Cast "operator"s?
enum class CastOperator {
  COP_StaticCast, COP_ConstCast, COP_BitCast
};

std::uint16_t PrecOf(BinaryOperator bop) noexcept;

} // namespace Syntax
} // namespace ckx

#endif // OPERATOR_H
