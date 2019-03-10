#ifndef UNRESOLVEDDECL_HPP
#define UNRESOLVEDDECL_HPP

#include "Syntax/CSTFwd.h"
#include "AST/DeclBase.hpp"

namespace ckx {
namespace Sema {

class Scope;

class UnresolvedVarDecl : public AST::Decl {
public:
  UnresolvedVarDecl(sona::string_ref const& denotingName,
                 sona::ref_ptr<Syntax::Decl> concreteDecl,
                 std::shared_ptr<Sema::Scope> const& scope,
                 sona::ref_ptr<AST::DeclContext> inContext)
    : AST::Decl(DK_Unresolved, inContext),
      m_DenotingName(denotingName),
      m_ConcreteDecl(concreteDecl),
      m_Scope(scope),
      m_InContext(inContext) {}

  sona::ref_ptr<Syntax::Decl> GetConcreteDecl() const noexcept {
    return m_ConcreteDecl;
  }

  void SelfReplace(sona::owner<AST::VarDecl> &&realDecl);

private:
  sona::string_ref m_DenotingName;
  sona::ref_ptr<Syntax::Decl> m_ConcreteDecl;
  std::shared_ptr<Sema::Scope> m_Scope;
  sona::ref_ptr<AST::DeclContext> m_InContext;
};

class HalfwayDecl : public AST::Decl {
public:
  HalfwayDecl(sona::string_ref const& denotingName,
              sona::ref_ptr<AST::Decl> halfwayDecl,
              std::shared_ptr<Sema::Scope> const& scope,
              sona::ref_ptr<AST::DeclContext> inContext)
    : AST::Decl(DK_Halfway, inContext),
      m_DenotingName(denotingName),
      m_HalfwayDecl(halfwayDecl),
      m_Scope(scope) {}

  void SelfReplace();

private:
  sona::string_ref m_DenotingName;
  sona::ref_ptr<AST::Decl> m_HalfwayDecl;
  std::shared_ptr<Sema::Scope> m_Scope;
};

} // namespace Sema
} // namespace ckx

#endif // UNRESOLVEDDECL_HPP
