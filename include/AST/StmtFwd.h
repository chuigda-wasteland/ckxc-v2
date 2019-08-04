#ifndef AST_STMTFWD_H
#define AST_STMTFWD_H

namespace ckx {
namespace AST {

#define AST_STMT_A(name) class name;
#define AST_STMT(name) class name;
#include "AST/Nodes.def"

} // namespace AST
} // namespace ckx

#endif // STMTFWD_H
