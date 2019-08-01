#include "Syntax/Operator.h"

namespace ckx {
namespace Syntax {

std::uint16_t PrecOf(Syntax::BinaryOperator bop) noexcept {
  switch (bop) {
  case Syntax::BinaryOperator::BOP_Mul:
  case Syntax::BinaryOperator::BOP_Div:
  case Syntax::BinaryOperator::BOP_Mod:
    return 40000;

  case Syntax::BinaryOperator::BOP_Add:
  case Syntax::BinaryOperator::BOP_Sub:
    return 35000;

  case Syntax::BinaryOperator::BOP_BitAnd:
  case Syntax::BinaryOperator::BOP_BitOr:
  case Syntax::BinaryOperator::BOP_BitXor:
    return 30000;

  case Syntax::BinaryOperator::BOP_Lt:
  case Syntax::BinaryOperator::BOP_Gt:
  case Syntax::BinaryOperator::BOP_LEq:
  case Syntax::BinaryOperator::BOP_GEq:
  case Syntax::BinaryOperator::BOP_Eq:
  case Syntax::BinaryOperator::BOP_NEq:
    return 25000;

  case Syntax::BinaryOperator::BOP_LogicAnd:
  case Syntax::BinaryOperator::BOP_LogicOr:
  case Syntax::BinaryOperator::BOP_LogicXor:
    return 20000;

  default:
    sona_unreachable();
  }

  return 0;
}

} // namespace Syntax
} // namespace ckx
