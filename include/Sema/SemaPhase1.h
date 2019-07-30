#ifndef SEMA_PHASE1_H
#define SEMA_PHASE1_H

#include "Sema/SemaCommon.h"
#include "sona/either.h"

#include "AST/Expr.h"

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
  void PostTranslateIncompleteVar(sona::ref_ptr<Sema::IncompleteVarDecl> iVar);
  void PostTranslateIncompleteTag(sona::ref_ptr<Sema::IncompleteTagDecl> iTag);
  void PostTranslateIncompleteValueCtor(
      sona::ref_ptr<Sema::IncompleteValueCtorDecl> iValueCtor);
  void PostTranslateIncompleteUsing(
      sona::ref_ptr<Sema::IncompleteUsingDecl> iusing);

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
  ActOnAlgebraic(sona::ref_ptr<Syntax::BinaryExpr const> concrete,
                 sona::owner<AST::Expr> &&lhs, sona::owner<AST::Expr> &&rhs,
                 Syntax::BinaryOperator bop);

  sona::owner<AST::Expr>
  ActOnLogic(sona::ref_ptr<Syntax::BinaryExpr const> concrete,
             sona::owner<AST::Expr> &&lhs, sona::owner<AST::Expr> &&rhs,
             Syntax::BinaryOperator bop);

  sona::owner<AST::Expr>
  ActOnBitwise(sona::ref_ptr<Syntax::BinaryExpr const> concrete,
               sona::owner<AST::Expr> &&lhs, sona::owner<AST::Expr> &&rhs,
               Syntax::BinaryOperator bop);

  sona::owner<AST::Expr>
  ActOnCompare(sona::ref_ptr<Syntax::BinaryExpr const> concrete,
               sona::owner<AST::Expr> &&lhs, sona::owner<AST::Expr> &&rhs,
               Syntax::BinaryOperator bop);

  sona::owner<AST::Expr>
  ActOnStaticCast(sona::ref_ptr<const Syntax::CastExpr> concrete,
                  sona::owner<AST::Expr> &&castedExpr, AST::QualType destType);
  
  sona::owner<AST::Expr>
  ActOnConstCast(sona::ref_ptr<const Syntax::CastExpr> concrete,
                 sona::owner<AST::Expr> &&castedExpr, AST::QualType destType);

  /// @note this function does not always "move away" or "consume" the input
  /// `castedExpr`, it only consumes its input when the cast is valid, and
  /// its return value is not nullptr. This'll get refactored by sometime, but
  /// let us keep it until we can make changes.
  sona::owner<AST::Expr>
  TryImplicitCast(sona::ref_ptr<Syntax::Expr const> concrete,
                  sona::owner<AST::Expr> &&castedExpr,
                  AST::QualType destType, bool shouldDiag = false);

  bool
  TryNumericPromotion(AST::Expr::ValueCat castedExprValueCat,
                      AST::QualType fromType, AST::QualType destType,
                      sona::ref_ptr<AST::BuiltinType const> fromBtin,
                      sona::ref_ptr<AST::BuiltinType const> destBtin,
                      std::vector<AST::CastStep> &outputVec);

  void DoNumericCast(AST::QualType fromType, AST::QualType destType,
                     sona::ref_ptr<AST::BuiltinType const> fromBtin,
                     sona::ref_ptr<AST::BuiltinType const> destBtin,
                     std::vector<AST::CastStep> &outputVec);

  bool
  TryPointerQualAdjust(AST::Expr::ValueCat fromValueCat, AST::QualType formType,
                       AST::QualType destType, bool isConstCast,
                       std::vector<AST::CastStep> &outputVec);
                       
  bool
  TryRefQualAdjust(AST::Expr::ValueCat fromValueCat, AST::QualType fromType,
                   AST::QualType destType, bool isConstCast,
                   std::vector<AST::CastStep> &outputVec);

  sona::owner<AST::Expr> LValueToRValueDecay(sona::owner<AST::Expr> &&expr);

  void LValueToRValueDecay(AST::Expr::ValueCat fromValueCat,
                           AST::QualType fromType,
                           std::vector<AST::CastStep> &outputVec);

  void AdjustTopLevelQual(AST::QualType fromType,
                          AST::QualType destType,
                          std::vector<AST::CastStep> &outputVec);

protected:
  /// @note this function does not always "move away" or "consume" the input
  /// `baseExpr`, it only consumes its input when the cast is valid, and
  /// its return value is not nullptr. This'll get refactored by sometime, but
  /// let us keep it until we can make changes.
  sona::owner<AST::Expr>
  TryFindUnaryOperatorOverload(std::shared_ptr<Scope> scope,
                               sona::owner<AST::Expr> &&baseExpr,
                               Syntax::UnaryOperator uop);

  /// @note this function does not always "move away" or "consume" the input
  /// `lhs` and `rhs`, it only consumes its input when the cast is valid, and
  /// its return value is not nullptr. This'll get refactored by sometime, but
  /// let us keep it until we can make changes.
  sona::owner<AST::Expr>
  TryFindBinaryOperatorOverload(std::shared_ptr<Scope> scope,
                                sona::owner<AST::Expr> &&lhs,
                                sona::owner<AST::Expr> &&rhs,
                                Syntax::BinaryOperator bop);

  sona::owner<AST::Expr>
  CreateOrAddImplicitCast(sona::owner<AST::Expr> &&expr,
                          AST::CastStep::CastStepKind castStepKind,
                          AST::QualType destType,
                          AST::Expr::ValueCat destValueCat);

  std::int8_t SIntRank(AST::BuiltinType::BuiltinTypeId btid);
  std::int8_t UIntRank(AST::BuiltinType::BuiltinTypeId btid);
  std::int8_t FloatRank(AST::BuiltinType::BuiltinTypeId btid);

#include "Syntax/Nodes.def"
};

} // namespace Sema
} // namespace ckx

#endif // SEMA_PHASE1_H
