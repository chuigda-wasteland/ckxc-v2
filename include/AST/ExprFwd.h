#ifndef AST_EXPRFWD_H
#define AST_EXPRFWD_H

namespace ckx {
namespace AST {


#define AST_EXPR_A(name) class name;
#define AST_EXPR(name) class name;
#include "AST/Nodes.def"

class CastStep;

} // namespace AST
} // namespace ckx

#endif // AST_EXPRFWD_H
