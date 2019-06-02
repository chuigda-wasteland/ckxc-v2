#ifndef CSTFWD_H
#define CSTFWD_H

namespace ckx {
namespace Syntax {

#define CST_ABSTRACT(name) class name;
#define CST_TRANSUNIT(name) class name;
#define CST_MISC(name) class name;
#define CST_TYPE(name) class name;
#define CST_DECL(name) class name;
#define CST_STMT(name) class name;
#define CST_EXPR(name) class name;

#include "Syntax/Nodes.def"

} // namespace Syntax
} // namespace ckx

#endif // CSTFWD_H
