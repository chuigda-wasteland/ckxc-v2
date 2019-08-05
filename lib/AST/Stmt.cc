#include "AST/Stmt.h"

namespace ckx {
namespace AST {

#define AST_STMT(name) \
sona::owner<Backend::ActionResult> \
name::Accept(sona::ref_ptr<Backend::StmtVisitor> visitor) const { \
  return visitor->Visit##name(this); \
}

} // namespace AST
} // namespace ckx
