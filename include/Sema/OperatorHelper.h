#ifndef SEMA_OPERATORHELPER_H
#define SEMA_OPERATORHELPER_H

#include "Syntax/Operator.h"
#include "AST/Expr.h"

namespace ckx {
namespace Sema {

const char *RepresentationOf(Syntax::BinaryOperator bop) noexcept;

AST::BinaryExpr::BinaryOperator
OperatorConv(Syntax::BinaryOperator bop) noexcept;

AST::UnaryExpr::UnaryOperator
OperatorConv(Syntax::UnaryOperator uop) noexcept;

} // namespace Sema
} // namespace ckx

#endif // SEMA_`OPERATORHELPER_H
