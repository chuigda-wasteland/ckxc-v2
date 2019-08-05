#include "AST/Expr.h"

namespace ckx {
namespace AST {

#define AST_EXPR(name) \
sona::owner<Backend::ActionResult> \
name::Accept(sona::ref_ptr<Backend::ExprVisitor> visitor) const { \
  return visitor->Visit##name(this); \
}
#include "AST/Nodes.def"

} // namespace AST
} // namespace ckx
