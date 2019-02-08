#ifndef SEMA_H
#define SEMA_H

#include "Sema/Scope.h"

#include "Basic/Diagnose.h"
#include "Syntax/CST.h"
#include "AST/DeclFwd.hpp"
#include "AST/ExprFwd.hpp"
#include "AST/StmtFwd.hpp"
#include "AST/TypeFwd.hpp"

#include "sona/pointer_plus.hpp"

namespace ckx {
namespace Sema {

class InScopeIdentifier final {
public:
  class IdScope {
  public:
    enum ScopeKind {
      ISK_InEnum,
      ISK_InStruct,
      ISK_InModule,
      ISK_InADT,
      ISK_Any
    };

    IdScope(sona::string_ref scopeName, ScopeKind scopeKind)
      : m_ScopeName(scopeName), m_ScopeKind(scopeKind) {}

    sona::string_ref const& GetScopeName() const noexcept {
      return m_ScopeName;
    }

    ScopeKind GetScopeKind() const noexcept {
      return m_ScopeKind;
    }

  private:
    sona::string_ref m_ScopeName;
    ScopeKind m_ScopeKind;
  };

  InScopeIdentifier(std::vector<IdScope> &&scope, sona::string_ref id)
    : m_Scope(std::move(scope)), m_Id(id) {}

  std::vector<IdScope> const& GetScope() const noexcept {
    return m_Scope;
  }

  sona::string_ref const& GetId() const noexcept {
    return m_Id;
  }

private:
  std::vector<IdScope> m_Scope;
  sona::string_ref m_Id;
};

class UnresolvedDeclaration final {
public:
  enum DependencyKind { UDK_Strong, UDK_Weak };
  using Dependency = std::pair<InScopeIdentifier, DependencyKind>;

private:
  std::vector<Dependency> m_Dependencies;
  std::shared_ptr<Scope> m_Scope;
  sona::ref_ptr<AST::DeclContext> m_InContext;
};

class SemaPhase0 final {
public:
  SemaPhase0(Diag::DiagnosticEngine &diag);

  sona::owner<AST::TransUnitDecl>
  ActOnTransUnit(sona::ref_ptr<Syntax::TransUnit> transUnit);

private:
  void PushScope(Scope::ScopeFlags flags = Scope::SF_None);
  void PopScope();

  sona::ref_ptr<AST::Type const>
  ResolveType(std::shared_ptr<Scope> scope,
              sona::ref_ptr<Syntax::Type const> type);

  sona::ref_ptr<AST::Decl>
  ActOnDecl(std::shared_ptr<Scope> scope,
            sona::ref_ptr<Syntax::Decl const> decl);

  sona::owner<AST::Stmt>
  ActOnStmt(std::shared_ptr<Scope> scope,
            sona::ref_ptr<Syntax::Stmt const> stmt);

  sona::owner<AST::Expr>
  ActOnExpr(std::shared_ptr<Scope> scope,
            sona::ref_ptr<Syntax::Expr const> expr);

#define CST_TYPE(name) \
  sona::ref_ptr<AST::Type const> \
  Resolve##name(std::shared_ptr<Scope> scope, \
                sona::ref_ptr<Syntax::name const> type);

#define CST_DECL(name) \
  sona::ref_ptr<AST::Decl> \
  ActOn##name(std::shared_ptr<Scope> scope, \
              sona::ref_ptr<Syntax::name const> decl);

  /*
#define CST_STMT(name) \
  sona::ref_ptr<AST::Stmt> \
  ActOn##name(std::shared_ptr<Scope> scope, \
              sona::ref_ptr<Syntax::name const> stmt);
              */

#define CST_EXPR(name) \
  sona::ref_ptr<AST::Expr> \
  ActOn##name(std::shared_ptr<Scope> scope, \
              sona::ref_ptr<Syntax::name const> expr);

#include "Syntax/CSTNodeDefs.def"

private:
  void PushDeclContext(sona::ref_ptr<AST::DeclContext> context);
  void PopDeclContext();
  sona::ref_ptr<AST::DeclContext> GetCurrentDeclContext();
  std::shared_ptr<Scope> GetCurrentScope();

  Diag::DiagnosticEngine &m_Diag;

  AST::ASTContext m_ASTContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> m_DeclContexts;
  std::vector<std::shared_ptr<Scope>> m_ScopeChains;
  std::vector<Syntax::Export> m_Exports;
};

} // namespace Sema
} // namespace ckx

#endif // CSTANNOTATOR_H
