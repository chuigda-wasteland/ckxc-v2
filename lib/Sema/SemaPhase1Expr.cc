#include "Sema/SemaPhase1.h"
#include "AST/Expr.h"
#include "Syntax/Concrete.h"

namespace ckx {
namespace Sema {

sona::owner<AST::Expr>
SemaPhase1::ActOnIntLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::IntLiteralExpr const> literalExpr) {
  AST::BuiltinType::BuiltinTypeId btid =
      ClassifyBuiltinTypeId(literalExpr->GetValue());
  return new AST::IntLiteralExpr(literalExpr->GetValue(),
                                 m_ASTContext.GetBuiltinType(btid));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnUIntLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::UIntLiteralExpr const> literalExpr) {
  AST::BuiltinType::BuiltinTypeId btid =
      ClassifyBuiltinTypeId(literalExpr->GetValue());
  return new AST::UIntLiteralExpr(literalExpr->GetValue(),
                                  m_ASTContext.GetBuiltinType(btid));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnFloatLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::FloatLiteralExpr const> literalExpr) {
  AST::BuiltinType::BuiltinTypeId btid =
      ClassifyBuiltinTypeId(literalExpr->GetValue());
  return new AST::FloatLiteralExpr(literalExpr->GetValue(),
                                   m_ASTContext.GetBuiltinType(btid));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnCharLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::CharLiteralExpr const> literalExpr) {
  return new AST::CharLiteralExpr(
             literalExpr->GetValue(),
             m_ASTContext.GetBuiltinType(AST::BuiltinType::BTI_Char));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnStringLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::StringLiteralExpr const> literalExpr) {
  AST::QualType charType =
      m_ASTContext.GetBuiltinType(AST::BuiltinType::BTI_Char);
  charType.AddConst();
  return new AST::StringLiteralExpr(
               literalExpr->GetValue(),
               m_ASTContext.CreatePointerType(charType));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnBoolLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::BoolLiteralExpr const> literalExpr) {
  return new AST::BoolLiteralExpr(
           literalExpr->GetValue(),
           m_ASTContext.GetBuiltinType(AST::BuiltinType::BTI_Bool));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnNullLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::NullLiteralExpr const>) {
  return new AST::NullptrLiteralExpr(
             m_ASTContext.GetBuiltinType(AST::BuiltinType::BTI_NilType));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnUnaryAlgebraicExpr(
    std::shared_ptr<Scope> scope,
    sona::ref_ptr<Syntax::UnaryAlgebraicExpr const> expr) {
  sona::owner<AST::Expr> baseExpr = ActOnExpr(scope, expr->GetBaseExpr());

  sona::owner<AST::Expr> maybeOverload =
      TryFindUnaryOperatorOverload(baseExpr.borrow(), expr->GetOperator());
  AST::QualType baseExprTy = baseExpr.borrow()->GetExprType();
  if (maybeOverload.borrow() != nullptr) {
    return maybeOverload;
  }

  switch (expr->GetOperator()) {
  case Syntax::UnaryOperator::UOP_Deref:
    if (baseExprTy.GetUnqualTy()->IsPointer()) {
      AST::QualType pointeeType =
          baseExprTy.GetUnqualTy()
                    .cast_unsafe<AST::PointerType const>()
                    ->GetPointee();
      return new AST::UnaryExpr(
            AST::UnaryExpr::UOP_Deref,
            LValueToRValueDecay(std::move(baseExpr)),
            pointeeType, AST::Expr::VC_LValue);
    }
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrOpRequiresType, {"*", "pointer"}),
                expr->GetOpRange());
    break;
  case Syntax::UnaryOperator::UOP_LogicNot:
    if (baseExprTy.GetUnqualTy()->IsBuiltin()) {
      sona::ref_ptr<AST::BuiltinType const> builtinTy =
          baseExprTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
      if (builtinTy->GetBtid()
          == AST::BuiltinType::BTI_Bool) {
        return new AST::UnaryExpr(
              AST::UnaryExpr::UOP_LogicalNot,
              LValueToRValueDecay(std::move(baseExpr)),
              m_ASTContext.GetBuiltinType(
                AST::BuiltinType::BTI_Bool),
              AST::Expr::VC_RValue);
      }
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrOpRequiresType, {"!", "boolean"}),
                  expr->GetOpRange());
    }
    break;
  case Syntax::UnaryOperator::UOP_Negative:
    if (baseExprTy.GetUnqualTy()->IsBuiltin()) {
      sona::ref_ptr<AST::BuiltinType const> builtinTy =
          baseExprTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
      if ((builtinTy->IsIntegral() && builtinTy->IsSigned())
          || builtinTy->IsFloating()) {
        return new AST::UnaryExpr(AST::UnaryExpr::UOP_Negative,
                                  LValueToRValueDecay(std::move(baseExpr)),
                                  baseExprTy, AST::Expr::VC_RValue);
      }
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrOpRequiresType,
                               {"-", "signed numeric"}),
                  expr->GetOpRange());
    }
    break;
  case Syntax::UnaryOperator::UOP_Positive:
    if (baseExprTy.GetUnqualTy()->IsBuiltin()) {
      sona::ref_ptr<AST::BuiltinType const> builtinTy =
          baseExprTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
      if (builtinTy->IsIntegral() || builtinTy->IsFloating()) {
        return new AST::UnaryExpr(AST::UnaryExpr::UOP_Positive,
                                  LValueToRValueDecay(std::move(baseExpr)),
                                  baseExprTy, AST::Expr::VC_RValue);
      }
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrOpRequiresType, {"+", "numeric"}),
                  expr->GetOpRange());
    }
    break;
  case Syntax::UnaryOperator::UOP_SelfIncr:
    if (baseExprTy.GetUnqualTy()->IsBuiltin()) {
      sona::ref_ptr<AST::BuiltinType const> builtinTy =
          baseExprTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
      if (builtinTy->IsIntegral()) {
        return new AST::UnaryExpr(AST::UnaryExpr::UOP_Incr,
                                  LValueToRValueDecay(std::move(baseExpr)),
                                  baseExprTy, AST::Expr::VC_RValue);
      }
    }
    else if (baseExprTy.GetUnqualTy()->IsPointer()) {
      return new AST::UnaryExpr(AST::UnaryExpr::UOP_Incr,
                                LValueToRValueDecay(std::move(baseExpr)),
                                baseExprTy, AST::Expr::VC_RValue);
    }
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrOpRequiresType,
                             {"++", "integral or pointer"}),
                expr->GetOpRange());
    break;
  case Syntax::UnaryOperator::UOP_SelfDecr:
    if (baseExprTy.GetUnqualTy()->IsBuiltin()) {
      sona::ref_ptr<AST::BuiltinType const> builtinTy =
          baseExprTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
      if (builtinTy->IsIntegral()) {
        return new AST::UnaryExpr(AST::UnaryExpr::UOP_Decr,
                                  LValueToRValueDecay(std::move(baseExpr)),
                                  baseExprTy, AST::Expr::VC_RValue);
      }
    }
    else if (baseExprTy.GetUnqualTy()->IsPointer()) {
      return new AST::UnaryExpr(AST::UnaryExpr::UOP_Decr,
                                LValueToRValueDecay(std::move(baseExpr)),
                                baseExprTy, AST::Expr::VC_RValue);
    }
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrOpRequiresType,
                             {"--", "integral or pointer"}),
                expr->GetOpRange());
    break;
  case Syntax::UnaryOperator::UOP_PointerTo:
    if (baseExpr.borrow()->GetValueCat() == AST::Expr::VC_LValue) {
      return new AST::UnaryExpr(AST::UnaryExpr::UOP_AddrOf,
                                std::move(baseExpr), baseExprTy,
                                AST::Expr::VC_RValue);
    }
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrAddressOfRValue, {}),
                expr->GetOpRange());
    break;
  case Syntax::UnaryOperator::UOP_BitReverse:
    if (baseExprTy.GetUnqualTy()->IsBuiltin()) {
      sona::ref_ptr<AST::BuiltinType const> builtinTy =
          baseExprTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
      if (builtinTy->IsIntegral() && builtinTy->IsUnsigned()) {
        return new AST::UnaryExpr(AST::UnaryExpr::UOP_BitwiseNot,
                                  LValueToRValueDecay(std::move(baseExpr)),
                                  baseExprTy, AST::Expr::VC_RValue);
      }
    }
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrOpRequiresType,
                             {"~", "unsigned int"}),
                expr->GetOpRange());
    break;
  }
  sona_unreachable();
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::ActOnCastExpr(std::shared_ptr<Scope> scope,
                          sona::ref_ptr<Syntax::CastExpr const> expr) {
  sona::owner<AST::Expr> castedExpr = ActOnExpr(scope, expr->GetCastedExpr());
  AST::QualType destType = ResolveType(scope, expr->GetDestType());
  switch (expr->GetOperator()) {
  case Syntax::CastOperator::COP_ConstCast: {
    if (castedExpr.borrow()->GetExprType().GetUnqualTy()
        != destType.GetUnqualTy()) {
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrConstCastDifferentBase, {}),
                  expr->GetCastOpRange());
    }
    return new AST::ExplicitCastExpr(AST::ExplicitCastExpr::ECOP_Const,
                                     std::move(castedExpr), destType,
                                     AST::Expr::VC_RValue);
  }
  case Syntax::CastOperator::COP_BitCast: {
    /// @todo need a method for calculating size of types
    sona_unreachable1("not implemented");
    return nullptr;
  }
  case Syntax::CastOperator::COP_StaticCast: {
    return ActOnStaticCast(expr, std::move(castedExpr), destType);
  }
  }
}

sona::owner<AST::Expr>
SemaPhase1::ActOnStaticCast(sona::ref_ptr<Syntax::CastExpr const> concrete,
                            sona::owner<AST::Expr> &&castedExpr,
                            AST::QualType destType) {
  if (castedExpr.borrow()->GetExprType() == destType) {
    m_Diag.Diag(Diag::DIR_Warning0,
                Diag::Format(Diag::DMT_WarnRedundantStatcCast, {}),
                concrete->GetCastOpRange());
    return std::move(castedExpr);
  }

  sona::owner<AST::Expr> implicitCastResult =
      TryImplicitCast(concrete.cast_unsafe<Syntax::Expr const>(),
                      std::move(castedExpr), destType);
  if (implicitCastResult.borrow() != nullptr) {
    m_Diag.Diag(Diag::DIR_Warning0,
                Diag::Format(Diag::DMT_WarnRedundantStatcCast, {}),
                concrete->GetCastOpRange());
    return implicitCastResult;
  }

  AST::QualType fromType = castedExpr.borrow()->GetExprType();
  AST::QualType fromTypeDequal = fromType.DeQual();
  sona::ref_ptr<AST::Type const> fromTypeUnqual = fromType.GetUnqualTy();
  sona::ref_ptr<AST::Type const> destTypeUnqual = destType.GetUnqualTy();

  std::vector<AST::CastStep> castSteps;
  switch (castedExpr.borrow()->GetValueCat()) {
  case AST::Expr::VC_LValue:
    castSteps.emplace_back(AST::CastStep::ICSK_LValue2RValue,
                           fromTypeDequal, AST::Expr::VC_RValue);
    break;
  case AST::Expr::VC_RValue:
    sona_assert(!fromType.GetCVR());
    break;
  case AST::Expr::VC_XValue:
    sona_unreachable1("not implemented");
  }

  if (fromTypeUnqual->IsBuiltin() && destTypeUnqual->IsBuiltin()) {
    sona::ref_ptr<AST::BuiltinType const> fromTypeBtin =
        fromTypeUnqual.cast_unsafe<AST::BuiltinType const>();
    sona::ref_ptr<AST::BuiltinType const> destTypeBtin =
        destTypeUnqual.cast_unsafe<AST::BuiltinType const>();
    if (fromTypeBtin->IsNumeric() && destTypeBtin->IsNumeric()) {
      DoNumericCast(fromType, destType, fromTypeBtin, destTypeBtin, castSteps);
      return new AST::ExplicitCastExpr(AST::ExplicitCastExpr::ECOP_Static,
                                       std::move(castedExpr),
                                       std::move(castSteps));
    }
  }

  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::TryImplicitCast(sona::ref_ptr<const Syntax::Expr> concrete,
                            sona::owner<AST::Expr> &&castedExpr,
                            AST::QualType destType, bool shouldDiag) {
  (void)concrete;

  AST::QualType fromType = castedExpr.borrow()->GetExprType();
  if (fromType == destType) {
    return std::move(castedExpr);
  }

  std::vector<AST::CastStep> castSteps;

  sona::ref_ptr<AST::Type const> fromTypeUnqual = fromType.GetUnqualTy();
  sona::ref_ptr<AST::Type const> destTypeUnqual = destType.GetUnqualTy();

  if (fromTypeUnqual->IsBuiltin()) {
    sona::ref_ptr<AST::BuiltinType const> fromBtin =
        fromTypeUnqual.cast_unsafe<AST::BuiltinType const>();
    if (fromBtin->GetBtid() == AST::BuiltinType::BTI_NilType
        && destTypeUnqual->IsPointer()) {
      sona_assert(castedExpr.borrow()->GetValueCat() == AST::Expr::VC_RValue);
      sona_assert(!castedExpr.borrow()->GetExprType().GetCVR());
      AST::QualType destTypeDequal(destTypeUnqual);
      castSteps.emplace_back(AST::CastStep::ICSK_Nil2Ptr,
                             destTypeDequal, AST::Expr::VC_RValue);
      if (destTypeDequal.GetCVR() != destType.GetCVR()) {
        castSteps.emplace_back(AST::CastStep::ICSK_AdjustQual,
                               destType, AST::Expr::VC_RValue);
      }
      return new AST::ImplicitCast(std::move(castedExpr), std::move(castSteps));
    }

    if (destTypeUnqual->IsBuiltin()) {
      sona::ref_ptr<AST::BuiltinType const> destBtin =
          destTypeUnqual.cast_unsafe<AST::BuiltinType const>();
      if (fromBtin->IsNumeric() && destBtin->IsNumeric()
          && TryNumericPromotion(
                 castedExpr.borrow()->GetValueCat(),
                 fromType, destType, fromBtin, destBtin, castSteps)) {
        return new AST::ImplicitCast(std::move(castedExpr),
                                     std::move(castSteps));
      }
    }
  }
  else if (fromTypeUnqual->IsPointer() && destTypeUnqual->IsPointer()
           && TryPointerQualAdjust(castedExpr.borrow()->GetValueCat(),
                                   fromType, destType, castSteps)) {
    return new AST::ImplicitCast(std::move(castedExpr), std::move(castSteps));
  }

  if (shouldDiag) {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrCannotImplicitCast,
                { "<not-implemented>", "<not-implemented>" }),
                /** @todo */ SourceRange(0, 0, 0));
  }

  return nullptr;
}

bool SemaPhase1::TryNumericPromotion(
    AST::Expr::ValueCat fromValueCat,
    AST::QualType fromType, AST::QualType destType,
    sona::ref_ptr<const AST::BuiltinType> fromBtin,
    sona::ref_ptr<const AST::BuiltinType> destBtin,
    std::vector<AST::CastStep> &outputVec) {
  /// @todo extract function
  switch (fromValueCat) {
  case AST::Expr::VC_LValue: {
    AST::QualType fromTypeDequal = fromType.DeQual();
    outputVec.emplace_back(AST::CastStep::ICSK_LValue2RValue,
                           fromTypeDequal, AST::Expr::VC_RValue);
    break;
  }
  case AST::Expr::VC_RValue:
    sona_assert(!fromType.GetCVR());
    break;
  case AST::Expr::VC_XValue:
    sona_unreachable1("not implemented");
    break;
  }

  AST::QualType destTypeDequal = destType.DeQual();
  if (fromBtin->IsSigned() && destBtin->IsSigned()
      && (SIntRank(fromBtin->GetBtid()) <= SIntRank(destBtin->GetBtid()))) {
    outputVec.emplace_back(AST::CastStep::ICSK_IntPromote,
                           destTypeDequal, AST::Expr::VC_RValue);
  }
  else if (fromBtin->IsUnsigned() && destBtin->IsUnsigned()
           && (UIntRank(fromBtin->GetBtid())
               <= UIntRank(destBtin->GetBtid()))) {
    outputVec.emplace_back(AST::CastStep::ICSK_UIntPromote,
                           destTypeDequal, AST::Expr::VC_RValue);
  }
  else if (fromBtin->IsFloating() && destBtin->IsFloating()
           && (FloatRank(fromBtin->GetBtid())
               <= FloatRank(destBtin->GetBtid()))) {
    outputVec.emplace_back(AST::CastStep::ICSK_FloatPromote,
                           destTypeDequal, AST::Expr::VC_RValue);
  }
  else {
    return false;
  }

  if (destTypeDequal.GetCVR() != destType.GetCVR()) {
    outputVec.emplace_back(AST::CastStep::ICSK_AdjustQual,
                           destTypeDequal, AST::Expr::VC_RValue);
  }
  return true;
}

void SemaPhase1::DoNumericCast(AST::QualType fromType, AST::QualType destType,
                               sona::ref_ptr<const AST::BuiltinType> fromBtin,
                               sona::ref_ptr<const AST::BuiltinType> destBtin,
                               std::vector<AST::CastStep> &outputVec) {
  (void)fromType;

  AST::QualType destTypeDequal = destType.DeQual();

  AST::CastStep::CastStepKind castStepKind;
  if (fromBtin->IsSigned() && destBtin->IsSigned()) {
    /// Since integral promotions should have been handled by implicit cast
    sona_assert(SIntRank(fromBtin->GetBtid()) > SIntRank(destBtin->GetBtid()));
    castStepKind = AST::CastStep::ECSK_IntDowngrade;
  }
  else if (fromBtin->IsUnsigned() && destBtin->IsUnsigned()) {
    sona_assert(UIntRank(fromBtin->GetBtid()) > UIntRank(destBtin->GetBtid()));
    castStepKind = AST::CastStep::ECSK_UIntDowngrade;
  }
  else if (fromBtin->IsFloating() && destBtin->IsFloating()) {
    sona_assert(FloatRank(fromBtin->GetBtid())
                > FloatRank(destBtin->GetBtid()));
    castStepKind = AST::CastStep::ECSK_FloatDowngrade;
  }
  else if (fromBtin->IsSigned() && destBtin->IsUnsigned()) {
    castStepKind = AST::CastStep::ECSK_Signed2Unsigned;
  }
  else if (fromBtin->IsUnsigned() && destBtin->IsSigned()) {
    castStepKind = AST::CastStep::ECSK_Unsigned2Signed;
  }
  else if (fromBtin->IsSigned() && destBtin->IsFloating()) {
    castStepKind = AST::CastStep::ECSK_Int2Float;
  }
  else if (fromBtin->IsUnsigned() && destBtin->IsFloating()) {
    castStepKind = AST::CastStep::ECSK_UInt2Float;
  }
  else if (fromBtin->IsFloating() && destBtin->IsSigned()) {
    castStepKind = AST::CastStep::ECSK_Float2Int;
  }
  else if (fromBtin->IsFloating() && destBtin->IsUnsigned()) {
    castStepKind = AST::CastStep::ECSK_FLoat2UInt;
  }

  outputVec.emplace_back(castStepKind, destTypeDequal, AST::Expr::VC_RValue);

  if (destTypeDequal.GetCVR() != destType.GetCVR()) {
    outputVec.emplace_back(AST::CastStep::ICSK_AdjustQual,
                           destType, AST::Expr::VC_RValue);
  }
}

bool SemaPhase1::TryPointerQualAdjust(
    AST::Expr::ValueCat fromValueCat,
    AST::QualType fromType, AST::QualType destType,
    std::vector<AST::CastStep> &outputVec) {
  LValueToRValueDecay(fromValueCat, fromType, outputVec);
  sona::ref_ptr<AST::PointerType const> fromPtrType =
      fromType.GetUnqualTy().cast_unsafe<AST::PointerType const>();
  sona::ref_ptr<AST::PointerType const> destPtrType =
      destType.GetUnqualTy().cast_unsafe<AST::PointerType const>();

  AST::QualType::QualCompareResult qcr =
      fromPtrType->GetPointee().CompareQualsWith(destPtrType->GetPointee());
  if (qcr == AST::QualType::CR_NoSense || qcr == AST::QualType::CR_MoreQual) {
    return false;
  }

  outputVec.emplace_back(AST::CastStep::ICSK_AdjustQual, destType.DeQual(),
                         AST::Expr::ValueCat::VC_RValue);
  return true;
}

void SemaPhase1::LValueToRValueDecay(AST::Expr::ValueCat fromValueCat,
                                     AST::QualType fromType,
                                     std::vector<AST::CastStep> &outputVec) {
  switch (fromValueCat) {
  case AST::Expr::VC_LValue: {
    AST::QualType fromTypeDequal = fromType.DeQual();
    outputVec.emplace_back(AST::CastStep::ICSK_LValue2RValue,
                           fromTypeDequal, AST::Expr::VC_RValue);
    break;
  }
  case AST::Expr::VC_RValue:
    sona_assert(!fromType.GetCVR());
    break;
  case AST::Expr::VC_XValue:
    sona_unreachable1("not implemented");
    break;
  }
}

sona::owner<AST::Expr>
SemaPhase1::CreateOrAddImplicitCast(sona::owner<AST::Expr> &&expr,
                                    AST::CastStep::CastStepKind castStepKind,
                                    AST::QualType destType,
                                    AST::Expr::ValueCat destValueCat) {
  if (expr.borrow()->GetExprId() == AST::Expr::ExprId::EI_ImplicitCast) {
    sona::owner<AST::ImplicitCast> castExpr =
        std::move(expr).cast_unsafe<AST::ImplicitCast>();
    AST::ImplicitCast *raw = std::move(castExpr).get();
    return std::move(*raw)
             .AddCastStep(AST::CastStep(castStepKind, destType, destValueCat))
             .cast_unsafe<AST::Expr>();
  }
  else {
    return new AST::ImplicitCast(
                 std::move(expr),
                 std::vector<AST::CastStep> {
                   AST::CastStep(castStepKind, destType, destValueCat) } );
  }
}

sona::owner<AST::Expr>
SemaPhase1::LValueToRValueDecay(sona::owner<AST::Expr> &&expr) {
  if (expr.borrow()->GetValueCat() == AST::Expr::VC_RValue) {
    return std::move(expr);
  }

  AST::QualType exprType = expr.borrow()->GetExprType().DeQual();

  return CreateOrAddImplicitCast(std::move(expr),
                                 AST::CastStep::ICSK_LValue2RValue,
                                 exprType, AST::Expr::VC_RValue);
}

sona::owner<AST::Expr>
SemaPhase1::TryFindUnaryOperatorOverload(
    sona::ref_ptr<const AST::Expr> baseExpr, Syntax::UnaryOperator uop) {
  (void)baseExpr;
  (void)uop;
  return nullptr;
}

} // namespace Sema
} // namespace ckx
