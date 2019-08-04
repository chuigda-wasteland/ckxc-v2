#include "Sema/SemaPhase1.h"
#include "Sema/OperatorHelper.h"
#include "AST/Expr.h"
#include "Syntax/Concrete.h"

namespace ckx {
namespace Sema {

sona::owner<AST::Expr>
SemaPhase1::ActOnIdRefExpr(std::shared_ptr<Scope> scope,
                           sona::ref_ptr<Syntax::IdRefExpr const> expr) {
  /// @todo replace this
  sona::ref_ptr<AST::VarDecl const> varDecl =
      scope->LookupVarDecl(expr->GetId().GetIdentifier());
  if (varDecl == nullptr) {
    /// @todo add diagnostics
    return nullptr;
  }
  return new AST::IdRefExpr(varDecl, varDecl->GetType(), AST::Expr::VC_LValue);
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
  return new AST::CharLiteralExpr(literalExpr->GetValue(),
                                  m_ASTContext.GetBuiltinType(
                                    AST::BuiltinType::BTI_Char));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnStringLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::StringLiteralExpr const> literalExpr) {
  AST::QualType charType =
      m_ASTContext.GetBuiltinType(AST::BuiltinType::BTI_Char);
  charType.AddConst();
  return new AST::StringLiteralExpr(literalExpr->GetValue(),
                                    m_ASTContext.CreatePointerType(charType));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnBoolLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::BoolLiteralExpr const> literalExpr) {
  return new AST::BoolLiteralExpr(literalExpr->GetValue(),
                                  m_ASTContext.GetBuiltinType(
                                    AST::BuiltinType::BTI_Bool));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnNullLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::NullLiteralExpr const>) {
  return new AST::NullptrLiteralExpr(
             m_ASTContext.GetBuiltinType(AST::BuiltinType::BTI_NilType));
}

sona::owner<AST::Expr>
SemaPhase1::ActOnAssignExpr(std::shared_ptr<Scope> scope,
                            sona::ref_ptr<Syntax::AssignExpr const> expr) {
  sona::owner<AST::Expr> lhs = ActOnExpr(scope, expr->GetLeftHandSide());
  sona::owner<AST::Expr> rhs = ActOnExpr(scope, expr->GetRightHandSide());
  if (lhs.borrow() == nullptr || rhs.borrow() == nullptr) {
    return nullptr;
  }

  sona::owner<AST::Expr> maybeOverload =
      TryFindAssignOperatorOverload(scope, std::move(lhs), std::move(rhs),
                                    expr->GetOperator());
  if (maybeOverload.borrow() != nullptr) {
    return maybeOverload;
  }

  if (lhs.borrow()->GetValueCat() != AST::Expr::VC_LValue) {
    /// @todo add diagnostics here
    return nullptr;
  }
  if (lhs.borrow()->GetExprType().IsConst()) {
    /// @todo add diagnostics here
    return nullptr;
  }

  switch (expr->GetOperator()) {
  case Syntax::AssignOperator::AOP_Assign:
    rhs = TryImplicitCast(nullptr, std::move(rhs),
                          lhs.borrow()->GetExprType().DeQual());
    if (rhs.borrow() == nullptr) {
      /// @todo add diagnostics here
      return nullptr;
    }
    return new AST::AssignExpr(AST::AssignExpr::AssignmentOperator::AOP_Assign,
                               std::move(lhs), std::move(rhs),
                               m_ASTContext.GetBuiltinType(
                                 AST::BuiltinType::BTI_Void),
                               AST::Expr::ValueCat::VC_RValue);

  default:
    sona_unreachable1("not implemented");
    return nullptr;
  }
}

sona::owner<AST::Expr>
SemaPhase1::ActOnBinaryExpr(std::shared_ptr<Scope> scope,
                            sona::ref_ptr<Syntax::BinaryExpr const> expr) {
  sona::owner<AST::Expr> lhs = ActOnExpr(scope, expr->GetLeftHandSide());
  sona::owner<AST::Expr> rhs = ActOnExpr(scope, expr->GetRightHandSide());
  if (lhs.borrow() == nullptr || rhs.borrow() == nullptr) {
    return nullptr;
  }

  sona::owner<AST::Expr> maybeOverload =
      TryFindBinaryOperatorOverload(scope, std::move(lhs), std::move(rhs),
                                    expr->GetOperator());
  if (maybeOverload.borrow() != nullptr) {
    return maybeOverload;
  }

  lhs = LValueToRValueDecay(std::move(lhs));
  rhs = LValueToRValueDecay(std::move(rhs));

  switch (expr->GetOperator()) {
  case Syntax::BinaryOperator::BOP_Add:
  case Syntax::BinaryOperator::BOP_Sub:
  case Syntax::BinaryOperator::BOP_Mul:
  case Syntax::BinaryOperator::BOP_Div:
  case Syntax::BinaryOperator::BOP_Mod:
    return ActOnAlgebraic(expr, std::move(lhs), std::move(rhs),
                          expr->GetOperator());

  case Syntax::BinaryOperator::BOP_LogicAnd:
  case Syntax::BinaryOperator::BOP_LogicOr:
  case Syntax::BinaryOperator::BOP_LogicXor:
    return ActOnLogic(expr, std::move(lhs), std::move(rhs),
                      expr->GetOperator());

  case Syntax::BinaryOperator::BOP_BitAnd:
  case Syntax::BinaryOperator::BOP_BitOr:
  case Syntax::BinaryOperator::BOP_BitXor:
    return ActOnBitwise(expr, std::move(lhs), std::move(rhs),
                        expr->GetOperator());

  case Syntax::BinaryOperator::BOP_BitLshift:
  case Syntax::BinaryOperator::BOP_BitRshift:
    return ActOnBitwiseShift(expr, std::move(lhs), std::move(rhs),
                             expr->GetOperator());

  case Syntax::BinaryOperator::BOP_Lt:
  case Syntax::BinaryOperator::BOP_Gt:
  case Syntax::BinaryOperator::BOP_Eq:
  case Syntax::BinaryOperator::BOP_LEq:
  case Syntax::BinaryOperator::BOP_GEq:
  case Syntax::BinaryOperator::BOP_NEq:
    return ActOnCompare(expr, std::move(lhs), std::move(rhs),
                        expr->GetOperator());

  case Syntax::BinaryOperator::BOP_Invalid:
    break;
  }

  sona_unreachable();
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::ActOnArraySubscriptExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<const Syntax::ArraySubscriptExpr>) {
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::ActOnMixFixExpr(std::shared_ptr<Scope>,
                            sona::ref_ptr<const Syntax::MixFixExpr>) {
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::ActOnFuncCallExpr(std::shared_ptr<Scope>,
                              sona::ref_ptr<Syntax::FuncCallExpr const>) {
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::ActOnMemberAccessExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::MemberAccessExpr const>) {
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::ActOnSizeOfExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::SizeOfExpr const>) {
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::ActOnAlignOfExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::AlignOfExpr const>) {
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::ActOnAlgebraic(sona::ref_ptr<const Syntax::BinaryExpr> concrete,
                           sona::owner<AST::Expr> &&lhs,
                           sona::owner<AST::Expr> &&rhs,
                           Syntax::BinaryOperator bop) {
  (void)concrete;

  AST::QualType lhsTy = lhs.borrow()->GetExprType();
  AST::QualType rhsTy = rhs.borrow()->GetExprType();

  switch (bop) {
  /// @todo here may be some duplicate codes, consider extracting methods
  case Syntax::BinaryOperator::BOP_Add:
    if (lhsTy.GetUnqualTy()->IsPointer() && rhsTy.GetUnqualTy()->IsBuiltin()
        && rhsTy.GetUnqualTy()
                .cast_unsafe<AST::BuiltinType const>()->IsIntegral()) {
      return new AST::BinaryExpr(AST::BinaryExpr::BOP_Add,
                                 std::move(lhs), std::move(rhs),
                                 lhsTy, AST::Expr::VC_RValue);
    }
    else if (lhsTy.GetUnqualTy()->IsBuiltin()
             && rhsTy.GetUnqualTy()->IsBuiltin()) {
      return ActOnNumeric(concrete, std::move(lhs), std::move(rhs), bop);
    }
    else {
      /// @todo add diagnostics info
      return nullptr;
    }

  case Syntax::BinaryOperator::BOP_Sub:
    if (lhsTy.GetUnqualTy()->IsPointer() && rhsTy.GetUnqualTy()->IsBuiltin()
        && rhsTy.GetUnqualTy()
                .cast_unsafe<AST::BuiltinType const>()->IsIntegral()) {
      return new AST::BinaryExpr(AST::BinaryExpr::BOP_Sub,
                                 std::move(lhs), std::move(rhs),
                                 lhsTy, AST::Expr::VC_RValue);
    }
    else if (lhsTy.GetUnqualTy()->IsPointer()
             && rhsTy.GetUnqualTy()->IsPointer()) {
      sona::ref_ptr<AST::PointerType const> lhsPointerTy =
          lhsTy.GetUnqualTy().cast_unsafe<AST::PointerType const>();
      sona::ref_ptr<AST::PointerType const> rhsPointerTy =
          rhsTy.GetUnqualTy().cast_unsafe<AST::PointerType const>();
      if (lhsPointerTy->GetPointee().GetUnqualTy()
          != rhsPointerTy->GetPointee().GetUnqualTy()) {
        /// @todo add diagnostics info
        return nullptr;
      }
      return new AST::BinaryExpr(AST::BinaryExpr::BOP_Sub,
                                 std::move(lhs), std::move(rhs),
                                 m_ASTContext.GetBuiltinType(
                                   AST::BuiltinType::BTI_UInt32),
                                  /// @todo add size_t, ssize_t,
                                  /// intptr_t, uintptr_t
                                 AST::Expr::VC_RValue);
    }
    else if (lhsTy.GetUnqualTy()->IsBuiltin()
             && rhsTy.GetUnqualTy()->IsBuiltin()) {
      return ActOnNumeric(concrete, std::move(lhs), std::move(rhs), bop);
    }
    else {
      /// @todo add diagnostics info
      return nullptr;
    }

  case Syntax::BinaryOperator::BOP_Mul:
  case Syntax::BinaryOperator::BOP_Div:
  case Syntax::BinaryOperator::BOP_Mod:
    return ActOnNumeric(concrete, std::move(lhs), std::move(rhs), bop);
    break;
  default: ;
  }

  sona_unreachable();
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::ActOnNumeric(sona::ref_ptr<const Syntax::BinaryExpr> concrete,
                         sona::owner<AST::Expr> &&lhs,
                         sona::owner<AST::Expr> &&rhs,
                         Syntax::BinaryOperator bop) {
  sona::ref_ptr<AST::BuiltinType const> lhsBuiltinTy =
      lhs.borrow()->GetExprType().GetUnqualTy()
         .cast_unsafe<AST::BuiltinType const>();
  sona::ref_ptr<AST::BuiltinType const> rhsBuiltinTy =
      rhs.borrow()->GetExprType().GetUnqualTy()
         .cast_unsafe<AST::BuiltinType const>();

  if (bop == Syntax::BinaryOperator::BOP_Mod
      && !rhsBuiltinTy->IsIntegral()) {
    /// @todo add diagnostics
    return nullptr;
  }

  sona::ref_ptr<AST::BuiltinType const> commonType =
      CommonNumericType(lhsBuiltinTy, rhsBuiltinTy);
  AST::QualType commonType1(commonType.cast_unsafe<AST::Type const>());
  if (commonType == nullptr) {
    /// @todo add diagnostics
    return nullptr;
  }
  lhs = TryImplicitCast(concrete->GetLeftHandSide(),
                        std::move(lhs), commonType1);
  rhs = TryImplicitCast(concrete->GetLeftHandSide(),
                        std::move(rhs), commonType1);
  sona_assert(lhs.borrow() != nullptr && rhs.borrow() != nullptr);
  return new AST::BinaryExpr(OperatorConv(bop), std::move(lhs), std::move(rhs),
                             commonType1, AST::Expr::VC_RValue);
}

sona::owner<AST::Expr>
SemaPhase1::ActOnLogic(sona::ref_ptr<const Syntax::BinaryExpr> concrete,
                       sona::owner<AST::Expr> &&lhs,
                       sona::owner<AST::Expr> &&rhs,
                       Syntax::BinaryOperator bop) {
  AST::QualType lhsTy = lhs.borrow()->GetExprType();
  AST::QualType rhsTy = rhs.borrow()->GetExprType();
  if (!lhsTy.GetUnqualTy()->IsBuiltin() || !rhsTy.GetUnqualTy()->IsBuiltin()) {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrOpRequiresType,
                             { RepresentationOf(bop), "boolean" }),
                concrete->GetOpRange());
    return nullptr;
  }

  sona::ref_ptr<AST::BuiltinType const> lhsBuiltinType =
      lhsTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
  sona::ref_ptr<AST::BuiltinType const> rhsBuiltinType =
      rhsTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
  if (lhsBuiltinType->GetBtid() != AST::BuiltinType::BTI_Bool
      || rhsBuiltinType->GetBtid() != AST::BuiltinType::BTI_Bool) {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrOpRequiresType,
                             { RepresentationOf(bop), "boolean" }),
                concrete->GetOpRange());
    return nullptr;
  }

  /// @todo consider extract function
  AST::BinaryExpr::BinaryOperator bop1 = OperatorConv(bop);
  return new AST::BinaryExpr(
              bop1, std::move(lhs), std::move(rhs),
              m_ASTContext.GetBuiltinType(AST::BuiltinType::BTI_Bool),
              AST::Expr::VC_RValue);
}

sona::owner<AST::Expr>
SemaPhase1::ActOnBitwise(sona::ref_ptr<const Syntax::BinaryExpr> concrete,
                         sona::owner<AST::Expr> &&lhs,
                         sona::owner<AST::Expr> &&rhs,
                         Syntax::BinaryOperator bop) {
  AST::QualType lhsTy = lhs.borrow()->GetExprType();
  AST::QualType rhsTy = rhs.borrow()->GetExprType();
  if (!lhsTy.GetUnqualTy()->IsBuiltin() || !rhsTy.GetUnqualTy()->IsBuiltin()) {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrOpRequiresType,
                             { RepresentationOf(bop), "unsigned" }),
                concrete->GetOpRange());
    return nullptr;
  }

  sona::ref_ptr<AST::BuiltinType const> lhsBuiltinType =
      lhsTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
  sona::ref_ptr<AST::BuiltinType const> rhsBuiltinType =
      rhsTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
  if (!lhsBuiltinType->IsUnsigned() || !rhsBuiltinType->IsUnsigned()) {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrOpRequiresType,
                             { RepresentationOf(bop), "unsigned" }),
                concrete->GetOpRange());
    return nullptr;
  }

  sona::ref_ptr<AST::BuiltinType const> commonType =
      CommonNumericType(lhsBuiltinType, rhsBuiltinType);
  sona::ref_ptr<AST::Type const> commonType1 =
      commonType.cast_unsafe<AST::Type const>();
  sona_assert(commonType != nullptr);
  sona::owner<AST::Expr> lhsCasted =
      TryImplicitCast(concrete.cast_unsafe<Syntax::Expr const>(),
                      std::move(lhs), AST::QualType(commonType1));
  sona::owner<AST::Expr> rhsCasted =
      TryImplicitCast(concrete.cast_unsafe<Syntax::Expr const>(),
                      std::move(rhs), AST::QualType(commonType1));
  sona_assert(lhsCasted.borrow() != nullptr);
  sona_assert(rhsCasted.borrow() != nullptr);

  /// @todo consider extract function
  AST::BinaryExpr::BinaryOperator bop1 = OperatorConv(bop);
  return new AST::BinaryExpr(
              bop1, std::move(lhsCasted), std::move(rhsCasted),
        AST::QualType(commonType1), AST::Expr::VC_RValue);
}

sona::owner<AST::Expr>
SemaPhase1::ActOnBitwiseShift(sona::ref_ptr<const Syntax::BinaryExpr> concrete,
                              sona::owner<AST::Expr> &&lhs,
                              sona::owner<AST::Expr> &&rhs,
                              Syntax::BinaryOperator bop) {
  (void)concrete;

  AST::BinaryExpr::BinaryOperator bop1 = OperatorConv(bop);
  AST::QualType lhsTy = lhs.borrow()->GetExprType();
  AST::QualType rhsTy = rhs.borrow()->GetExprType();
  if (!lhsTy.GetUnqualTy()->IsBuiltin() || !rhsTy.GetUnqualTy()->IsBuiltin()) {
    /// @todo add diags info
    return nullptr;
  }

  sona::ref_ptr<AST::BuiltinType const> lhsBuiltinType =
      lhsTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
  sona::ref_ptr<AST::BuiltinType const> rhsBuiltinType =
      rhsTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
  if (!lhsBuiltinType->IsUnsigned() || !rhsBuiltinType->IsUnsigned()) {
    /// @todo add diags info
    return nullptr;
  }

  return new AST::BinaryExpr(bop1, std::move(lhs), std::move(rhs),
                             lhsTy, AST::Expr::ValueCat::VC_RValue);
}

sona::owner<AST::Expr>
SemaPhase1::ActOnCompare(sona::ref_ptr<const Syntax::BinaryExpr> concrete,
                         sona::owner<AST::Expr> &&lhs,
                         sona::owner<AST::Expr> &&rhs,
                         Syntax::BinaryOperator bop) {
  AST::BinaryExpr::BinaryOperator bop1 = OperatorConv(bop);
  AST::QualType lhsTy = lhs.borrow()->GetExprType();
  AST::QualType rhsTy = rhs.borrow()->GetExprType();
  if (lhsTy.GetUnqualTy()->IsBuiltin() && rhsTy.GetUnqualTy()->IsBuiltin()) {
    sona::ref_ptr<AST::BuiltinType const> lhsBuiltinType =
        lhsTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
    sona::ref_ptr<AST::BuiltinType const> rhsBuiltinType =
        rhsTy.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
    if (lhsBuiltinType->IsNumeric() && rhsBuiltinType->IsNumeric()) {
      sona::ref_ptr<AST::BuiltinType const> commonType =
          CommonNumericType(lhsBuiltinType, rhsBuiltinType);
      if (commonType == nullptr) {
        /// @todo add diagnostics here
        return nullptr;
      }
      sona::ref_ptr<AST::Type const> commonType1 =
          commonType.cast_unsafe<AST::Type const>();
      sona::owner<AST::Expr> lhsCasted =
          TryImplicitCast(concrete.cast_unsafe<Syntax::Expr const>(),
                          std::move(lhs), AST::QualType(commonType1));
      sona::owner<AST::Expr> rhsCasted =
          TryImplicitCast(concrete.cast_unsafe<Syntax::Expr const>(),
                          std::move(rhs), AST::QualType(commonType1));
      sona_assert(lhsCasted.borrow() != nullptr);
      sona_assert(rhsCasted.borrow() != nullptr);
      lhsCasted = LValueToRValueDecay(std::move(lhsCasted));
      rhsCasted = LValueToRValueDecay(std::move(rhsCasted));

      return new AST::BinaryExpr(
                  bop1, std::move(lhsCasted), std::move(rhsCasted),
                  m_ASTContext.GetBuiltinType(AST::BuiltinType::BTI_Bool),
                  AST::Expr::VC_RValue);
    }
  }
  else if (lhsTy.GetUnqualTy()->IsPointer()
           && rhsTy.GetUnqualTy()->IsPointer()) {
    sona::ref_ptr<AST::PointerType const> lhsPtrType =
        lhsTy.GetUnqualTy().cast_unsafe<AST::PointerType const>();
    sona::ref_ptr<AST::PointerType const> rhsPtrType =
        rhsTy.GetUnqualTy().cast_unsafe<AST::PointerType const>();
    if (lhsPtrType->GetPointee().GetUnqualTy()
        != rhsPtrType->GetPointee().GetUnqualTy()) {
      /// @todo add diagnostics here
      return nullptr;
    }
    return new AST::BinaryExpr(
                bop1, std::move(lhs), std::move(rhs),
                m_ASTContext.GetBuiltinType(AST::BuiltinType::BTI_Bool),
                AST::Expr::VC_RValue);
  }

  /// @todo add diagnostics here
  return nullptr;
}

/// @todo this functions seems to be too long
sona::owner<AST::Expr>
SemaPhase1::ActOnUnaryAlgebraicExpr(
    std::shared_ptr<Scope> scope,
    sona::ref_ptr<Syntax::UnaryAlgebraicExpr const> expr) {
  sona::owner<AST::Expr> baseExpr = ActOnExpr(scope, expr->GetBaseExpr());
  if (baseExpr.borrow() == nullptr) {
    return nullptr;
  }

  sona::owner<AST::Expr> maybeOverload =
      TryFindUnaryOperatorOverload(scope, std::move(baseExpr),
                                   expr->GetOperator());
  if (maybeOverload.borrow() != nullptr) {
    return maybeOverload;
  }

  AST::QualType baseExprTy = baseExpr.borrow()->GetExprType();
  switch (expr->GetOperator()) {
  case Syntax::UnaryOperator::UOP_Deref:
    if (baseExprTy.GetUnqualTy()->IsPointer()) {
      AST::QualType pointeeType =
          baseExprTy.GetUnqualTy()
                    .cast_unsafe<AST::PointerType const>()
                    ->GetPointee();
      return new AST::UnaryExpr(AST::UnaryExpr::UOP_Deref,
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
        return new AST::UnaryExpr(AST::UnaryExpr::UOP_LogicNot,
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
        return new AST::UnaryExpr(AST::UnaryExpr::UOP_SelfIncr,
                                  LValueToRValueDecay(std::move(baseExpr)),
                                  baseExprTy, AST::Expr::VC_RValue);
      }
    }
    else if (baseExprTy.GetUnqualTy()->IsPointer()) {
      return new AST::UnaryExpr(AST::UnaryExpr::UOP_SelfIncr,
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
        return new AST::UnaryExpr(AST::UnaryExpr::UOP_SelfDecr,
                                  LValueToRValueDecay(std::move(baseExpr)),
                                  baseExprTy, AST::Expr::VC_RValue);
      }
    }
    else if (baseExprTy.GetUnqualTy()->IsPointer()) {
      return new AST::UnaryExpr(AST::UnaryExpr::UOP_SelfDecr,
                                LValueToRValueDecay(std::move(baseExpr)),
                                baseExprTy, AST::Expr::VC_RValue);
    }
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrOpRequiresType,
                             {"--", "integral or pointer"}),
                expr->GetOpRange());
    break;
  case Syntax::UnaryOperator::UOP_AddrOf:
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
        return new AST::UnaryExpr(AST::UnaryExpr::UOP_BitReverse,
                                  LValueToRValueDecay(std::move(baseExpr)),
                                  baseExprTy, AST::Expr::VC_RValue);
      }
    }
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrOpRequiresType,
                             {"~", "unsigned int"}),
                expr->GetOpRange());
    break;
  case Syntax::UnaryOperator::UOP_Invalid: ;
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
    return ActOnConstCast(expr, std::move(castedExpr), destType);
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

  castedExpr = LValueToRValueDecay(std::move(castedExpr));

  std::vector<AST::CastStep> castSteps;
  AST::QualType fromType = castedExpr.borrow()->GetExprType();
  sona::ref_ptr<AST::Type const> fromTypeUnqual = fromType.GetUnqualTy();
  sona::ref_ptr<AST::Type const> destTypeUnqual = destType.GetUnqualTy();

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

  m_Diag.Diag(Diag::DIR_Error,
              Diag::Format(Diag::DMT_ErrCannotStaticCast,
                           {"<not-implemented>", "<not-implementd>"}),
              SourceRange(0, 0, 0));
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::ActOnConstCast(sona::ref_ptr<const Syntax::CastExpr> concrete,
                           sona::owner<AST::Expr> &&castedExpr, 
                           AST::QualType destType) {
  (void)concrete;

  AST::QualType fromType = castedExpr.borrow()->GetExprType();
  std::vector<AST::CastStep> castSteps;
  if (fromType.GetUnqualTy()->IsPointer()
      && destType.GetUnqualTy()->IsPointer()) {
    DoPointerQualAdjust(destType, castSteps);
    return new AST::ExplicitCastExpr(AST::ExplicitCastExpr::ECOP_Const,
                                     std::move(castedExpr),
                                     std::move(castSteps));
  }
  else if (destType.GetUnqualTy()->IsReference()) {
    sona::ref_ptr<AST::RefType const> destRefType =
        destType.GetUnqualTy().cast_unsafe<AST::RefType const>();
    DoRefQualAdjust(destRefType->GetReferencedType(), castSteps);
    return new AST::ExplicitCastExpr(AST::ExplicitCastExpr::ECOP_Const,
                                     std::move(castedExpr),
                                     std::move(castSteps));
  }
  
  sona_unreachable1("not implemented");
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

  sona::ref_ptr<AST::Type const> fromTypeUnqual = fromType.GetUnqualTy();
  sona::ref_ptr<AST::Type const> destTypeUnqual = destType.GetUnqualTy();

  if (fromTypeUnqual == destTypeUnqual) {
    return LValueToRValueDecay(std::move(castedExpr));
  }

  if (fromTypeUnqual->IsBuiltin()) {
    sona::ref_ptr<AST::BuiltinType const> fromBtin =
        fromTypeUnqual.cast_unsafe<AST::BuiltinType const>();
    if (fromBtin->GetBtid() == AST::BuiltinType::BTI_NilType
        && destTypeUnqual->IsPointer()) {
      sona_assert(castedExpr.borrow()->GetValueCat() == AST::Expr::VC_RValue);
      sona_assert(!castedExpr.borrow()->GetExprType().GetCVR());
      AST::QualType destTypeDequal = destType.DeQual();
      return CreateOrAddImplicitCast(
            std::move(castedExpr), AST::CastStep::ICSK_Nil2Ptr,
            destTypeDequal, AST::Expr::VC_RValue);
    }

    if (destTypeUnqual->IsBuiltin()) {
      sona::ref_ptr<AST::BuiltinType const> fromBtin =
          fromTypeUnqual.cast_unsafe<AST::BuiltinType const>();
      sona::ref_ptr<AST::BuiltinType const> destBtin =
          destTypeUnqual.cast_unsafe<AST::BuiltinType const>();
      if (fromBtin->IsNumeric() && destBtin->IsNumeric()) {
        return TryNumericPromotion(std::move(castedExpr), destType,
                                   fromBtin, destBtin, shouldDiag);
      }
    }
  }
  else if (fromTypeUnqual->IsPointer() && destTypeUnqual->IsPointer()) {
    sona::ref_ptr<AST::PointerType const> fromPtr =
        fromTypeUnqual.cast_unsafe<AST::PointerType const>();
    sona::ref_ptr<AST::PointerType const> destPtr =
        destTypeUnqual.cast_unsafe<AST::PointerType const>();
    if (fromPtr->GetPointee().GetUnqualTy()
        == destPtr->GetPointee().GetUnqualTy()) {
      return TryPointerQualAdjust(std::move(castedExpr), destType,
                                  fromPtr, destPtr, shouldDiag);
    }
  }
  else if (destTypeUnqual->IsReference()) {
    /// @note not implemented yet since we don't know if this is useful.
    sona_unreachable1("not implemented since we don't know if this is useful.");
  }

  if (shouldDiag) {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrCannotImplicitCast,
                { "<not-implemented>", "<not-implemented>" }),
                /** @todo */ SourceRange(0, 0, 0));
  }

  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::TryNumericPromotion(sona::owner<AST::Expr> &&fromExpr,
                                AST::QualType destType,
                                sona::ref_ptr<const AST::BuiltinType> fromBtin,
                                sona::ref_ptr<const AST::BuiltinType> destBtin,
                                bool shouldDiag) {
  AST::QualType destTypeDequal = destType.DeQual();
  if (fromBtin->IsSigned() && destBtin->IsSigned()
      && (SIntRank(fromBtin->GetBtid()) <= SIntRank(destBtin->GetBtid()))) {
    return CreateOrAddImplicitCast(LValueToRValueDecay(std::move(fromExpr)),
                                   AST::CastStep::ICSK_IntPromote,
                                   destTypeDequal, AST::Expr::VC_RValue);
  }
  else if (fromBtin->IsUnsigned() && destBtin->IsUnsigned()
           && (UIntRank(fromBtin->GetBtid())
               <= UIntRank(destBtin->GetBtid()))) {
    return CreateOrAddImplicitCast(LValueToRValueDecay(std::move(fromExpr)),
                                   AST::CastStep::ICSK_UIntPromote,
                                   destTypeDequal, AST::Expr::VC_RValue);
  }
  else if (fromBtin->IsFloating() && destBtin->IsFloating()
           && (FloatRank(fromBtin->GetBtid())
               <= FloatRank(destBtin->GetBtid()))) {
    return CreateOrAddImplicitCast(LValueToRValueDecay(std::move(fromExpr)),
                                   AST::CastStep::ICSK_FloatPromote,
                                   destTypeDequal, AST::Expr::VC_RValue);
  }
  else {
    if (shouldDiag) {
      /// @todo add diagnostics here
    }
    return nullptr;
  }
}

sona::owner<AST::Expr>
SemaPhase1::TryPointerQualAdjust(sona::owner<AST::Expr> &&fromExpr,
                                 AST::QualType destType,
                                 sona::ref_ptr<const AST::PointerType> fromPtr,
                                 sona::ref_ptr<const AST::PointerType> destPtr,
                                 bool shouldDiag) {
  AST::QualType::QualCompareResult qcr =
    fromPtr->GetPointee().CompareQualsWith(destPtr->GetPointee());
  if (qcr == AST::QualType::CR_NoSense || qcr == AST::QualType::CR_MoreQual) {
    if (shouldDiag) {
      /// @todo add diagnostics here
    }
    return nullptr;
  }

  return CreateOrAddImplicitCast(LValueToRValueDecay(std::move(fromExpr)),
                                 AST::CastStep::CSK_AdjustPtrQual,
                                 destType.DeQual(),
                                 AST::Expr::ValueCat::VC_RValue);
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
}

/// @todo consider inflating these functions
void SemaPhase1::DoPointerQualAdjust(
    AST::QualType destType, std::vector<AST::CastStep> &outputVec) {
  outputVec.emplace_back(AST::CastStep::CSK_AdjustPtrQual, destType.DeQual(),
                         AST::Expr::ValueCat::VC_RValue);
}

void SemaPhase1::DoRefQualAdjust(AST::QualType destType,
                                 std::vector<AST::CastStep> &outputVec) {
  outputVec.emplace_back(AST::CastStep::CSK_AdjustRefQual, destType,
                         AST::Expr::ValueCat::VC_LValue);
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
SemaPhase1::TryFindUnaryOperatorOverload(std::shared_ptr<Scope> scope,
                                         sona::owner<AST::Expr> &&baseExpr,
                                         Syntax::UnaryOperator uop) {
  (void)scope;
  (void)baseExpr;
  (void)uop;
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::TryFindBinaryOperatorOverload(std::shared_ptr<Scope> scope,
                                          sona::owner<AST::Expr> &&lhs,
                                          sona::owner<AST::Expr> &&rhs,
                                          Syntax::BinaryOperator bop) {
  (void)scope;
  (void)lhs;
  (void)rhs;
  (void)bop;
  return nullptr;
}

sona::owner<AST::Expr>
SemaPhase1::TryFindAssignOperatorOverload(std::shared_ptr<Scope> scope,
                                          sona::owner<AST::Expr> &&lhs,
                                          sona::owner<AST::Expr> &&rhs,
                                          Syntax::AssignOperator aop) {
  (void)scope;
  (void)lhs;
  (void)rhs;
  (void)aop;
  return nullptr;
}

} // namespace Sema
} // namespace ckx
