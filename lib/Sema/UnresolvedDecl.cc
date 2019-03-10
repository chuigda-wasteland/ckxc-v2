#include "Sema/UnresolvedDecl.hpp"
#include "Sema/Scope.h"
#include "AST/Type.hpp"

namespace ckx {
namespace Sema {

void UnresolvedVarDecl::SelfReplace(sona::owner<AST::VarDecl>&& realDecl) {
  m_Scope->ReplaceVarDecl(m_DenotingName, realDecl.borrow());
  m_InContext->ReplaceDecl(this, realDecl.cast_unsafe<AST::Decl>());
}

/// @todo this implementation looks ugly, replace it at some appropriate type
void HalfwayDecl::SelfReplace() {
  if (m_HalfwayDecl->GetDeclKind() == DeclKind::DK_Class) {
    auto ty = m_ASTContext->AddUserDefinedType(
                new AST::ClassType(
                  m_DenotingName, m_HalfwayDecl.cast_unsafe<AST::ClassDecl>()));
    m_Scope->AddType(m_DenotingName, ty);
  }
  else /** @note preserved for `enum class' ADTs */ {
    sona_unreachable1("not implemented");
  }
}

} // namespace Sema
} // namespace ckx
