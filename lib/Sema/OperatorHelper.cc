#include "Sema/OperatorHelper.h"

namespace ckx {
namespace Sema {

/// @todo consider tablegen this function
char const* RepresentationOf(Syntax::BinaryOperator bop) noexcept {
  switch (bop) {
  case Syntax::BinaryOperator::BOP_Mul: return "*";
  case Syntax::BinaryOperator::BOP_Div: return "/";
  case Syntax::BinaryOperator::BOP_Mod: return "%";
  case Syntax::BinaryOperator::BOP_Add: return "+";
  case Syntax::BinaryOperator::BOP_Sub: return "-";

  case Syntax::BinaryOperator::BOP_BitAnd: return "&";
  case Syntax::BinaryOperator::BOP_BitOr:  return "|";
  case Syntax::BinaryOperator::BOP_BitXor: return "^";

  case Syntax::BinaryOperator::BOP_Lt:  return "<";
  case Syntax::BinaryOperator::BOP_Gt:  return ">";
  case Syntax::BinaryOperator::BOP_LEq: return "<=";
  case Syntax::BinaryOperator::BOP_GEq: return ">=";
  case Syntax::BinaryOperator::BOP_Eq:  return "==";
  case Syntax::BinaryOperator::BOP_NEq: return "!=";

  case Syntax::BinaryOperator::BOP_LogicAnd: return "&&";
  case Syntax::BinaryOperator::BOP_LogicOr:  return "||";
  case Syntax::BinaryOperator::BOP_LogicXor: return "^^";

  default:
    {
      sona_unreachable();
      return "<not-implemented>";
    }
  }
}

}
}
