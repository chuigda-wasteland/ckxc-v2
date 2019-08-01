#include "Sema/OperatorHelper.h"

namespace ckx {
namespace Sema {

char const* RepresentationOf(Syntax::BinaryOperator bop) noexcept {
  switch (bop) {
#define BINARY_OP_DEF(name, rep, text) \
  case Syntax::BinaryOperator::BOP_##name: return rep;
#include "Syntax/Operators.def"
  case Syntax::BinaryOperator::BOP_Invalid: ;
  }

  sona_unreachable();
  return "<not-implemented>";
}

/// @note since we use tablegen to generate operator tags, we can simply
/// cast them according to their numeric values.

AST::BinaryExpr::BinaryOperator
OperatorConv(Syntax::BinaryOperator bop) noexcept {
  return static_cast<AST::BinaryExpr::BinaryOperator>(bop);
}

AST::UnaryExpr::UnaryOperator
OperatorConv(Syntax::UnaryOperator uop) noexcept {
  return static_cast<AST::UnaryExpr::UnaryOperator>(uop);
}

} // namespace Sema
} // namespace ckx
