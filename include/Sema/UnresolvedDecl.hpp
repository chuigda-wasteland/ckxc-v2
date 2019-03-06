#ifndef UNRESOLVEDDECL_HPP
#define UNRESOLVEDDECL_HPP

#include "Syntax/CSTFwd.h"
#include "AST/DeclBase.hpp"
#include "Sema/Scope.h"

namespace ckx {

class UnresolvedDecl : public AST::Decl {
public:
  UnresolvedDecl(sona::ref_ptr<Syntax::Decl> concreteDecl,
                 std::shared_ptr<Sema::Scope> const& scope,
                 sona::ref_ptr<AST::DeclContext> inContext)
    : AST::Decl(DK_Unresolved, inContext),
      m_ConcreteDecl(concreteDecl),
      m_Scope(scope),
      m_InContext(inContext) {}

  sona::ref_ptr<Syntax::Decl> GetConcreteDecl() const noexcept {
    return m_ConcreteDecl;
  }

  void SelfReplace(sona::owner<AST::Decl> &&realDecl) const noexcept;

private:
  sona::ref_ptr<Syntax::Decl> m_ConcreteDecl;
  std::shared_ptr<Sema::Scope> m_Scope;
  sona::ref_ptr<AST::DeclContext> m_InContext;
};

}

#endif // UNRESOLVEDDECL_HPP
