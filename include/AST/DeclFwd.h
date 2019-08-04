#ifndef AST_DECLFWD_H
#define AST_DECLFWD_H

namespace ckx {
namespace AST {

class DeclContext;

#define AST_DECL_A(name) class name;
#define AST_DECL(name) class name;
#include "AST/Nodes.def"

} // namespace AST
} // namespace ckx

#endif // AST_DECLFWD_H
