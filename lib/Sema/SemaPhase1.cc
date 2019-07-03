#include "Sema/SemaPhase1.h"

#include "AST/Expr.h"
#include "Syntax/Concrete.h"

namespace ckx {
namespace Sema {

SemaPhase1::SemaPhase1(AST::ASTContext &astContext,
    std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
    Diag::DiagnosticEngine &diag)
  : SemaCommon(astContext, declContexts, diag) {}

void SemaPhase1::PostTranslateIncompletes(
    std::vector<sona::ref_ptr<IncompleteDecl>> incompletes) {
  for (auto incomplete : incompletes) {
    switch (incomplete->GetType()) {
    case IncompleteDecl::IDT_Var:
      PostTranslateIncompleteVar(incomplete.cast_unsafe<IncompleteVarDecl>());
      break;
    case IncompleteDecl::IDT_Tag:
      PostTranslateIncompleteTag(incomplete.cast_unsafe<IncompleteTagDecl>());
      break;
    case IncompleteDecl::IDT_ValueCtor:
      PostTranslateIncompleteADTConstructor(
            incomplete.cast_unsafe<IncompleteValueCtorDecl>());
      break;
    case IncompleteDecl::IDT_Using:
      PostTranslateIncompleteUsing(
            incomplete.cast_unsafe<IncompleteUsingDecl>());
      break;
    case IncompleteDecl::IDT_Function:
      sona_unreachable1("functions should not be solved here");
      break;
    }
  }
}

AST::QualType SemaPhase1::ResolveType(std::shared_ptr<Scope> scope,
                        sona::ref_ptr<const Syntax::Type> type) {
  switch (type->GetNodeKind()) {
#define CST_TYPE(name) \
  case Syntax::Node::CNK_##name: \
    return Resolve##name(scope, type.cast_unsafe<Syntax::name const>());
#include "Syntax/Nodes.def"
  default:
    sona_unreachable();
  }
  return sona::ref_ptr<AST::Type const>(nullptr);
}

sona::owner<AST::Expr>
SemaPhase1::ActOnExpr(std::shared_ptr<Scope> scope,
                      sona::ref_ptr<const Syntax::Expr> expr) {
  (void)scope;
  (void)expr;
  sona_unreachable1("not implemented");
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::TryFindUnaryOperatorOverload(
    sona::ref_ptr<const AST::Expr> baseExpr, Syntax::UnaryOperator uop) {
  (void)baseExpr;
  (void)uop;
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::LValueToRValueDecay(sona::owner<AST::Expr> &&expr) {
  if (expr.borrow()->GetValueCat() == AST::Expr::VC_RValue) {
    return std::move(expr);
  }

  AST::QualType exprType = expr.borrow()->GetExprType();

  return CreateOrAddImplicitCast(std::move(expr),
                                 AST::CastStep::ICSK_LValue2RValue,
                                 exprType, AST::Expr::VC_RValue);
}

sona::owner<AST::Expr>
SemaPhase1::SignedIntPromote(sona::owner<AST::Expr> &&expr,
                             AST::BuiltinType::BuiltinTypeId destRank) {
  sona_assert(expr.borrow()->GetExprType().GetUnqualTy()->IsBuiltin()
              && expr.borrow()->GetExprType().GetUnqualTy()
                     .cast_unsafe<AST::BuiltinType const>()->IsSigned());
  sona_assert(AST::BuiltinType::IsSigned(destRank));

  sona::ref_ptr<AST::BuiltinType const> exprBuiltinType =
      expr.borrow()->GetExprType().GetUnqualTy()
          .cast_unsafe<AST::BuiltinType const>();

  sona_assert(AST::BuiltinType::SIntRank(
                  exprBuiltinType->GetBuiltinTypeId())
              <= AST::BuiltinType::SIntRank(destRank));

  if (AST::BuiltinType::SIntRank(exprBuiltinType->GetBuiltinTypeId())
      == AST::BuiltinType::SIntRank(destRank)) {
    return std::move(expr);
  }

  return CreateOrAddImplicitCast(std::move(expr),
                                 AST::CastStep::ICSK_IntPromote,
                                 /// @todo we need something to "imitate"
                                 /// the original type
                                 m_ASTContext.GetBuiltinType(destRank),
                                 AST::Expr::VC_RValue);
}

sona::owner<AST::Expr>
SemaPhase1::UnsignedIntPromote(sona::owner<AST::Expr> &&expr,
                               AST::BuiltinType::BuiltinTypeId destRank) {
  sona_assert(expr.borrow()->GetExprType().GetUnqualTy()->IsBuiltin()
              && expr.borrow()->GetExprType().GetUnqualTy()
                     .cast_unsafe<AST::BuiltinType const>()->IsUnsigned());
  sona_assert(AST::BuiltinType::IsUnsigned(destRank));

  sona::ref_ptr<AST::BuiltinType const> exprBuiltinType =
      expr.borrow()->GetExprType().GetUnqualTy()
          .cast_unsafe<AST::BuiltinType const>();

  sona_assert(AST::BuiltinType::UIntRank(
                  exprBuiltinType->GetBuiltinTypeId())
              <= AST::BuiltinType::UIntRank(destRank));

  if (AST::BuiltinType::UIntRank(exprBuiltinType->GetBuiltinTypeId())
      == AST::BuiltinType::UIntRank(destRank)) {
    return std::move(expr);
  }

  return CreateOrAddImplicitCast(std::move(expr),
                                 AST::CastStep::ICSK_IntPromote,
                                 m_ASTContext.GetBuiltinType(destRank),
                                 AST::Expr::VC_RValue);
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

void SemaPhase1::PostTranslateIncompleteVar(
    sona::ref_ptr<IncompleteVarDecl> iVar) {
  AST::QualType varType =
      ResolveType(iVar->GetEnclosingScope(), iVar->GetConcrete()->GetType());
  sona_assert(varType.GetUnqualTy() != nullptr);
  iVar->GetIncomplete()->SetType(varType);
}

void SemaPhase1::PostTranslateIncompleteTag(sona::ref_ptr<IncompleteTagDecl>) {
  // do nothing since the tag will be completed as its fields get completed
}

void SemaPhase1::PostTranslateIncompleteADTConstructor(
    sona::ref_ptr<IncompleteValueCtorDecl> iAdtC) {
  sona::ref_ptr<AST::ValueCtorDecl> halfway =
      iAdtC->GetHalfway().cast_unsafe<AST::ValueCtorDecl>();
  AST::QualType adtConstructorType =
      ResolveType(iAdtC->GetEnclosingScope(),
                  iAdtC->GetConcrete()->GetUnderlyingType());
  halfway->SetType(adtConstructorType);
}

void SemaPhase1::PostTranslateIncompleteUsing(
    sona::ref_ptr<IncompleteUsingDecl> iusing) {
  AST::QualType aliasee = ResolveType(iusing->GetEnclosingScope(),
                                      iusing->GetConcrete()->GetAliasee());
  iusing->GetHalfway()->FillAliasee(aliasee);
}

sona::owner<AST::Expr>
SemaPhase1::ActOnStaticCast(std::shared_ptr<Scope> scope,
                            sona::ref_ptr<Syntax::CastExpr const> concrete,
                            sona::owner<AST::Expr> &&castedExpr,
                            AST::QualType destType) {
  /// @todo try an implicit cast first. if the implicit cast works, emit an
  /// warning about the misuse of static cast

  (void)scope;
  sona_unreachable1("not implemented");

  AST::QualType fromType = castedExpr.borrow()->GetExprType();
  sona::ref_ptr<AST::Type const> fromTypeUnqual = fromType.GetUnqualTy();
  sona::ref_ptr<AST::Type const> destTypeUnqual = destType.GetUnqualTy();

  if (fromType == destType) {
    m_Diag.Diag(Diag::DIR_Warning0,
                Diag::Format(Diag::DMT_WarnRedundantStatcCast, {}),
                concrete->GetCastOpRange());
    return std::move(castedExpr);
  }

  /// @todo check volatile and restrict once we decide to support that
  if (fromType.IsConst() && !destType.IsConst()) {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrStaticCastDiscardConst, {}),
                concrete->GetCastOpRange());
    return nullptr;
  }

  if (fromTypeUnqual->IsPointer()
      && destTypeUnqual->IsBuiltin()
      && (destTypeUnqual.cast_unsafe<AST::BuiltinType const>()
                       ->GetBuiltinTypeId()
          == AST::BuiltinType::BTI_NilType)) {
  }
  else if (fromTypeUnqual->IsBuiltin()
           && destTypeUnqual->IsPointer()
           && (fromTypeUnqual.cast_unsafe<AST::BuiltinType const>()
                             ->GetBuiltinTypeId()
                == AST::BuiltinType::BTI_NilType)) {
  }
  else if (fromTypeUnqual->IsBuiltin() && destTypeUnqual->IsBuiltin()) {
    sona::ref_ptr<AST::BuiltinType const> fromTypeBtin =
        fromTypeUnqual.cast_unsafe<AST::BuiltinType const>();
    sona::ref_ptr<AST::BuiltinType const> destTypeBtin =
        destTypeUnqual.cast_unsafe<AST::BuiltinType const>();
    if (fromTypeBtin->IsNumeric() && destTypeBtin->IsNumeric()) {

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

  if (fromType.IsConst() && !destType.IsConst()) {
    if (shouldDiag) {
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrImplicitCastDiscardConst, {}),
                  /** @todo we need the concept of "representative" range */
                  SourceRange(0, 0, 0));
    }
    return nullptr;
  }

  std::vector<AST::CastStep> castSteps;
  AST::QualType currentType = fromType;
  AST::Expr::ValueCat currentValueCat = castedExpr.borrow()->GetValueCat();

  if (!fromType.IsConst() && destType.IsConst()) {
    currentType.AddConst();
    castSteps.emplace_back(AST::CastStep::ICSK_AddConst,
                           currentType, currentValueCat);
  }

  sona::ref_ptr<AST::Type const> curTypeUnqual = currentType.GetUnqualTy();
  sona::ref_ptr<AST::Type const> destTypeUnqual = destType.GetUnqualTy();

  if (curTypeUnqual->IsBuiltin()) {
    sona::ref_ptr<AST::BuiltinType const> curBtin =
        curTypeUnqual.cast_unsafe<AST::BuiltinType const>();
    if (curBtin->GetBuiltinTypeId()
        == AST::BuiltinType::BTI_NilType
        && destTypeUnqual->IsPointer()) {
      castSteps.emplace_back(AST::CastStep::ICSK_Nil2Ptr,
                             destType, AST::Expr::VC_RValue);
      return new AST::ImplicitCast(std::move(castedExpr), std::move(castSteps));
    }

    if (destTypeUnqual->IsBuiltin()) {
      sona::ref_ptr<AST::BuiltinType const> destBtin =
          destTypeUnqual.cast_unsafe<AST::BuiltinType const>();
      if (curBtin->IsNumeric() && destBtin->IsNumeric()
          && TryImplicitNumericCast(currentType, destType,
                                    curBtin, destBtin, castSteps)) {
        return new AST::ImplicitCast(std::move(castedExpr),
                                     std::move(castSteps));
      }
    }
  }

  m_Diag.Diag(Diag::DIR_Error,
              Diag::Format(Diag::DMT_ErrCannotImplicitCast,
              { "<not-implemented>", "<not-implemented>" }),
              /** @todo */ SourceRange(0, 0, 0));

  return nullptr;
}

bool SemaPhase1::TryImplicitNumericCast(
    AST::QualType fromType, AST::QualType destType,
    sona::ref_ptr<const AST::BuiltinType> fromBtin,
    sona::ref_ptr<const AST::BuiltinType> destBtin,
    std::vector<AST::CastStep> &outputVec) {
  (void)fromType;

  if (fromBtin->IsSigned() && destBtin->IsSigned()
      && (AST::BuiltinType::SIntRank(fromBtin->GetBuiltinTypeId())
          < AST::BuiltinType::SIntRank(destBtin->GetBuiltinTypeId()))) {
    outputVec.emplace_back(AST::CastStep::ICSK_IntPromote, destType,
                           AST::Expr::VC_RValue);
    return true;
  }
  else if (fromBtin->IsUnsigned() && destBtin->IsUnsigned()
           && (AST::BuiltinType::UIntRank(fromBtin->GetBuiltinTypeId())
               < AST::BuiltinType::UIntRank(destBtin->GetBuiltinTypeId()))) {
    outputVec.emplace_back(AST::CastStep::ICSK_UIntPromote, destType,
                           AST::Expr::VC_RValue);
    return true;
  }
  else if (fromBtin->IsFloating() && destBtin->IsFloating()
           && (AST::BuiltinType::FloatRank(fromBtin->GetBuiltinTypeId())
               < AST::BuiltinType::FloatRank(destBtin->GetBuiltinTypeId()))) {
    outputVec.emplace_back(AST::CastStep::ICSK_FloatPromote, destType,
                           AST::Expr::VC_RValue);
    return true;
  }

  return false;
}

AST::QualType
SemaPhase1::ResolveBuiltinType(std::shared_ptr<Scope>,
                               sona::ref_ptr<Syntax::BuiltinType const> bty) {
  return SemaCommon::ResolveBuiltinTypeImpl(bty);
}

AST::QualType
SemaPhase1::
ResolveUserDefinedType(std::shared_ptr<Scope> scope,
                       sona::ref_ptr<Syntax::UserDefinedType const> uty) {
  AST::QualType lookupResult = LookupType(scope, uty->GetName(), false);
  sona_assert(lookupResult.GetUnqualTy() != nullptr);
  return lookupResult;
}

AST::QualType
SemaPhase1::ResolveTemplatedType(std::shared_ptr<Scope>,
                                 sona::ref_ptr<Syntax::TemplatedType const>) {
  sona_unreachable1("not implemented");
  return sona::ref_ptr<AST::Type const>(nullptr);
}

AST::QualType
SemaPhase1::ResolveComposedType(std::shared_ptr<Scope> scope,
                                sona::ref_ptr<Syntax::ComposedType const> cty) {
  AST::QualType ret = ResolveType(scope, cty->GetRootType());

  /// @todo duplicate codes, remove them at some time.
  auto r = sona::linq::from_container(cty->GetTypeSpecifiers())
            .zip_with(
             sona::linq::from_container(cty->GetTypeSpecRanges()));
  for (const auto& p : r) {
    switch (p.first) {
    case Syntax::ComposedType::CTS_Pointer:
      ret = m_ASTContext.CreatePointerType(ret);
      break;
    case Syntax::ComposedType::CTS_Ref:
      ret = m_ASTContext.CreateLValueRefType(ret);
      break;
    case Syntax::ComposedType::CTS_RvRef:
      ret = m_ASTContext.CreateRValueRefType(ret);
      break;
    case Syntax::ComposedType::CTS_Const:
      if (ret.IsConst()) {
        m_Diag.Diag(Diag::DIR_Error,
                    Diag::Format(Diag::DMT_ErrDuplicateQual, {"const"}),
                    p.second);
      }
      else {
        ret.AddConst();
      }
      break;
    case Syntax::ComposedType::CTS_Volatile:
      if (ret.IsVolatile()) {
        m_Diag.Diag(Diag::DIR_Error,
                    Diag::Format(Diag::DMT_ErrDuplicateQual, {"volatile"}),
                    p.second);
      }
      else {
        ret.AddVolatile();
      }
      break;
    case Syntax::ComposedType::CTS_Restrict:
      if (ret.IsRestrict()) {
        m_Diag.Diag(Diag::DIR_Error,
                    Diag::Format(Diag::DMT_ErrDuplicateQual, {"restrict"}),
                    p.second);
      }
      else {
        ret.AddRestrict();
      }
      break;
    default:
      sona_unreachable1("not implemented");
    }
  }
  return ret;
}

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
      if (builtinTy->GetBuiltinTypeId()
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
    return ActOnStaticCast(scope, expr, std::move(castedExpr), destType);
  }
  }
}

} // namespace Sema
} // namespace ckx
