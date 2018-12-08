#ifndef SEMA_H
#define SEMA_H

#include "Sema/Scope.h"

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
  sona::owner<AST::TransUnitDecl>
  ActOnTransUnit(sona::ref_ptr<Syntax::TransUnit> transUnit);

private:
  void PushScope(Scope::ScopeFlags flags = Scope::SF_None);
  void PopScope();

  sona::ref_ptr<AST::Type const>
  ResolveType(sona::ref_ptr<Syntax::Type const> type);

  sona::owner<AST::Decl>
  ActOnDecl(sona::ref_ptr<Syntax::Decl const> decl);

  sona::owner<AST::Stmt>
  ActOnStmt(sona::ref_ptr<Syntax::Stmt const> stmt);

  sona::owner<AST::Expr>
  ActOnExpr(sona::ref_ptr<Syntax::Expr const> expr);

#define CST_TYPE(name) \
  sona::ref_ptr<AST::Type const> \
  Resolve##name(sona::ref_ptr<Syntax::name const> type);

#define CST_DECL(name) \
  sona::owner<AST::Decl> \
  ActOn##name(sona::ref_ptr<Syntax::name const> decl);

#define CST_STMT(name) \
  sona::owner<AST::Stmt> \
  ActOn##name(sona::ref_ptr<Syntax::name const> stmt);

#define CST_EXPR(name) \
  sona::owner<AST::Expr> \
  ActOn##name(sona::ref_ptr<Syntax::name const> expr);

#include "Syntax/CSTNodeDefs.def"

private:
  AST::ASTContext m_ASTContext;
  std::vector<Scope> m_ScopeChains;
  std::vector<Syntax::Export> m_Exports;
};

} // namespace Sema
} // namespace ckx

#endif // CSTANNOTATOR_H