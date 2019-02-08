#ifndef SEMA_H
#define SEMA_H

#include "Sema/Scope.h"

#include "Basic/Diagnose.h"
#include "Syntax/CSTFwd.h"
#include "AST/DeclFwd.hpp"
#include "AST/ExprFwd.hpp"
#include "AST/StmtFwd.hpp"
#include "AST/TypeFwd.hpp"

#include "sona/pointer_plus.hpp"

namespace ckx {
namespace Sema {

class SemaClass final {
public:
  SemaClass(Diag::DiagnosticEngine &diag);

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

#define CST_STMT(name) \
  sona::ref_ptr<AST::Stmt> \
  ActOn##name(std::shared_ptr<Scope> scope, \
              sona::ref_ptr<Syntax::name const> stmt);

#define CST_EXPR(name) \
  sona::ref_ptr<AST::Expr> \
  ActOn##name(std::shared_ptr<Scope> scope, \
              sona::ref_ptr<Syntax::name const> expr);

#include "Syntax/CSTNodeDefs.def"

private:
  void PushDeclContext(sona::ref_ptr<AST::DeclContext> context);
  void PopDeclContext();
  sona::ref_ptr<AST::DeclContext> GetCurrentDeclContext();
  sona::ref_ptr<Scope> GetCurrentScope();

  Diag::DiagnosticEngine &m_Diag;

  AST::ASTContext m_ASTContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> m_DeclContexts;
  std::vector<std::shared_ptr<Scope>> m_ScopeChains;
  std::vector<Syntax::Export> m_Exports;
};

} // namespace Sema
} // namespace ckx

#endif // CSTANNOTATOR_H
