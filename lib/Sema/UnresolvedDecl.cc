#include "Sema/UnresolvedDecl.hpp"
#include "Sema/Scope.h"

namespace ckx {
namespace Sema {

void UnresolvedVarDecl::SelfReplace(sona::owner<AST::VarDecl>&& realDecl) {
  m_Scope->ReplaceVarDecl(m_DenotingName, realDecl.borrow());
  m_InContext->ReplaceDecl(this, realDecl.cast_unsafe<AST::Decl>());
}

void HalfwayDecl::SelfReplace() {
}

} // namespace Sema
} // namespace ckx
