#ifndef AST_TYPEFWD_H
#define AST_TYPEFWD_H

namespace ckx {
namespace AST {

#define AST_TYPE_A(name) class name;
#define AST_TYPE(name) class name;
#include "AST/Nodes.def"

} // namespace AST
} // namespace ckx

#endif // TYPEFWD_H
