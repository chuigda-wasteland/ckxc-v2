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
  case BinaryOperator::BOP_BitNot:
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
  case BinaryOperator::BOP_LogicNot:
  case BinaryOperator::BOP_LogicXor:
    return 20000;

  default:
    sona_unreachable();
  }
}

UnaryOperator TokenToUnary(Frontend::Token::TokenKind token) noexcept {
  switch (token) {
  case Frontend::Token::TK_SYM_PLUS: return UnaryOperator::UOP_Positive;
  case Frontend::Token::TK_SYM_MINUS: return UnaryOperator::UOP_Negative;
  case Frontend::Token::TK_SYM_ASTER: return UnaryOperator::UOP_Deref;
  case Frontend::Token::TK_SYM_AMP: return UnaryOperator::UOP_PointerTo;

  default:
    sona_unreachable();
  }
}

BinaryOperator TokenToBinary(Frontend::Token::TokenKind token) noexcept {
  switch (token) {
  case Frontend::Token::TK_SYM_PLUS:  return BinaryOperator::BOP_Add;
  case Frontend::Token::TK_SYM_MINUS: return BinaryOperator::BOP_Sub;
  case Frontend::Token::TK_SYM_ASTER: return BinaryOperator::BOP_Mul;

  default:
    sona_unreachable();
  }
}

} // namespace Syntax
} // namespace ckx
