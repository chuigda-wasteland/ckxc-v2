#include "Syntax/Operator.h"

namespace ckx {
namespace Syntax {

std::uint16_t PrecOf(BinaryOperator bop) noexcept {
  switch (bop) {
  case BinaryOperator::BOP_Mul:
  case BinaryOperator::BOP_Div:
  case BinaryOperator::BOP_Mod:
    return 40000;

  case BinaryOperator::BOP_Add:
  case BinaryOperator::BOP_Sub:
    return 35000;

  case BinaryOperator::BOP_BitAnd:
  case BinaryOperator::BOP_BitOr:
  case BinaryOperator::BOP_BitXor:
    return 30000;

  case BinaryOperator::BOP_Lt:
  case BinaryOperator::BOP_Gt:
  case BinaryOperator::BOP_LEq:
  case BinaryOperator::BOP_GEq:
  case BinaryOperator::BOP_Eq:
  case BinaryOperator::BOP_NEq:
    return 25000;

  case BinaryOperator::BOP_LogicAnd:
  case BinaryOperator::BOP_LogicOr:
  case BinaryOperator::BOP_LogicXor:
    return 20000;

  default:
    sona_unreachable();
  }

  return 0;
}

} // namespace Syntax
} // namespace ckx
