#ifndef SEMA_PHASE1_H
#define SEMA_PHASE1_H

#include "Sema/SemaCommon.h"
#include "sona/either.h"

namespace ckx {
namespace Sema {

class SemaPhase1 : public SemaCommon {
public:
  SemaPhase1(AST::ASTContext &astContext,
             std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
             Diag::DiagnosticEngine &diag);

  void PostTranslateIncompletes(
      std::vector<sona::ref_ptr<Sema::IncompleteDecl>> incompletes);

  void TranslateFunctions(std::vector<IncompleteFuncDecl> &funcs);

protected:
  AST::QualType ResolveType(std::shared_ptr<Scope> scope,
                            sona::ref_ptr<Syntax::Type const> type);
  sona::owner<AST::Expr> ActOnExpr(std::shared_ptr<Scope> scope,
                                   sona::ref_ptr<Syntax::Expr const> expr);

#define CST_TYPE(name) \
  AST::QualType \
  Resolve##name(std::shared_ptr<Scope> scope, \
                sona::ref_ptr<Syntax::name const> type);

#define CST_EXPR(name) \
  sona::owner<AST::Expr> \
  ActOn##name(std::shared_ptr<Scope> scope, \
              sona::ref_ptr<Syntax::name const> type);

protected:
  sona::owner<AST::Expr>
  ActOnStaticCast(std::shared_ptr<Scope> scope,
                  sona::ref_ptr<const Syntax::CastExpr> concrete,
                  sona::owner<AST::Expr> &&castedExpr,
                  AST::QualType destType);

  sona::owner<AST::Expr>
  TryImplicitCast(sona::ref_ptr<Syntax::Expr const> concrete,
                  sona::owner<AST::Expr> &&castedExpr,
                  AST::QualType destType, bool shouldDiag = false);

  bool
  TryImplicitNumericCast(AST::QualType fromType, AST::QualType destType,
                         sona::ref_ptr<AST::BuiltinType const> fromBtin,
                         sona::ref_ptr<AST::BuiltinType const> destBtin,
                         std::vector<AST::CastStep> &outputVec);

  sona::owner<AST::Expr> LValueToRValueDecay(sona::owner<AST::Expr> &&expr);

protected:
  sona::owner<AST::Expr>
  TryFindUnaryOperatorOverload(sona::ref_ptr<AST::Expr const> baseExpr,
                               Syntax::UnaryOperator uop);

  sona::owner<AST::Expr>
  SignedIntPromote(sona::owner<AST::Expr> &&expr,
                   AST::BuiltinType::BuiltinTypeId destRank);
  sona::owner<AST::Expr>
  UnsignedIntPromote(sona::owner<AST::Expr> &&expr,
                     AST::BuiltinType::BuiltinTypeId destRank);

  sona::owner<AST::Expr>
  CreateOrAddImplicitCast(sona::owner<AST::Expr> &&expr,
                          AST::CastStepKind castStepKind,
                          AST::QualType destType,
                          AST::Expr::ValueCat destValueCat);

  void PostTranslateIncompleteVar(sona::ref_ptr<Sema::IncompleteVarDecl> iVar);
  void PostTranslateIncompleteTag(sona::ref_ptr<Sema::IncompleteTagDecl> iTag);
  void PostTranslateIncompleteADTConstructor(
      sona::ref_ptr<Sema::IncompleteValueCtorDecl> iAdtC);
  void PostTranslateIncompleteUsing(
      sona::ref_ptr<Sema::IncompleteUsingDecl> iusing);

#include "Syntax/Nodes.def"
};

} // namespace Sema
} // namespace ckx

#endif // SEMA_PHASE1_H
