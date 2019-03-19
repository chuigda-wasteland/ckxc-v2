#ifndef UNRESOLVEDDECL_HPP
#define UNRESOLVEDDECL_HPP

#include "Sema/Dependency.h"
#include "Syntax/CSTFwd.h"
#include "AST/Decl.hpp"


namespace ckx {
namespace Sema {

class Scope;

class IncompleteDecl {
public:
  IncompleteDecl(std::vector<Dependency> &&dependencies,
                 std::shared_ptr<Scope> const& inScope) :
    m_Dependencies(std::move(dependencies)),
    m_InScope(inScope) {}

  std::vector<Dependency>& GetDependencies() noexcept {
    return m_Dependencies;
  }

  std::vector<Dependency> const& GetDependencies() const noexcept {
    return m_Dependencies;
  }

  void AddNameDepend(Syntax::Identifier &&id, bool isStrong);
  void AddValueDepend(sona::ref_ptr<AST::Decl> decl, bool isStrong);

  std::shared_ptr<Scope> & GetEnclosingScope() noexcept {
    return m_InScope;
  }

private:
  std::vector<Dependency> m_Dependencies;
  std::shared_ptr<Scope> m_InScope;
};

class IncompleteVarDecl : public IncompleteDecl {
public:
  IncompleteVarDecl(sona::ref_ptr<Syntax::VarDecl> concrete,
                    sona::ref_ptr<AST::DeclContext> inContext,
                    std::vector<Dependency> &&dependencies,
                    std::shared_ptr<Scope> const& inScope)
    : IncompleteDecl(std::move(dependencies), inScope),
      m_Concrete(concrete), m_InContext(inContext) {}

  sona::ref_ptr<Syntax::VarDecl const> GetConcrete() const noexcept {
    return m_Concrete;
  }

  sona::ref_ptr<AST::DeclContext> GetDeclContext() noexcept {
    return m_InContext;
  }

private:
  sona::ref_ptr<Syntax::VarDecl> m_Concrete ;
  sona::ref_ptr<AST::DeclContext> m_InContext;
};

class IncompleteTagDecl : public IncompleteDecl {
public:
  IncompleteTagDecl(sona::ref_ptr<AST::Decl> halfway,
                    std::vector<Dependency> &&dependencies,
                    std::shared_ptr<Scope> const& inScope)
    : IncompleteDecl(std::move(dependencies), inScope), m_Halfway(halfway) {
    sona_assert(halfway->GetDeclKind() == AST::Decl::DK_Class
                /* || halfway->GetDeclKind() == AST::Decl::DK_ADT */);
  }

  sona::ref_ptr<AST::Decl const> GetHalfway() const noexcept {
    return m_Halfway;
  }

private:
  sona::ref_ptr<AST::Decl> m_Halfway;
};

class IncompleteUsingDecl : public IncompleteDecl {
public:
  IncompleteUsingDecl(sona::ref_ptr<AST::UsingDecl> halfway,
                      std::shared_ptr<Scope> const& inScope,
                      std::vector<Dependency> &&dependencies)
    : IncompleteDecl(std::move(dependencies), inScope), m_Halfway(halfway) {}

  sona::ref_ptr<AST::UsingDecl const> GetHalfway() const noexcept {
    return m_Halfway;
  }

private:
  sona::ref_ptr<AST::UsingDecl> m_Halfway;
};

} // namespace Sema
} // namespace ckx

#endif // UNRESOLVEDDECL_HPP
