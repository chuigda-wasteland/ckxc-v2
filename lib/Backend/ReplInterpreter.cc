#include "Backend/ReplInterpreter.h"

#include "sona/util.h"

namespace ckx {
namespace Backend {

sona::owner<ActionResult>
ReplInterpreter::VisitAssignExpr(sona::ref_ptr<AST::AssignExpr const> expr) {
  ReplValue assigned = expr->GetAssigned()->Accept(this).borrow()
                           ->GetValue<ReplValue>();
  ReplValue assignee = expr->GetAssignee()->Accept(this).borrow()
                           ->GetValue<ReplValue>();

  assigned.GetPtrValue().get() = assignee;

  return CreateResult(VoidType());
}

sona::owner<ActionResult>
ReplInterpreter::VisitUnaryExpr(sona::ref_ptr<AST::UnaryExpr const> expr) {
  ReplValue operand = expr->GetOperand()->Accept(this).borrow()
                          ->GetValue<ReplValue>();
  sona::ref_ptr<AST::Expr const> exprOperand = expr->GetOperand();
  sona::ref_ptr<AST::BuiltinType const> exprOperandTy =
      exprOperand->GetExprType().GetUnqualTy()
                                .cast_unsafe<AST::BuiltinType const>();
  switch (expr->GetOperator()) {
  case AST::UnaryExpr::UOP_SelfIncr:
    if (exprOperandTy->IsSigned()) {
      int64_t i = operand.GetPtrValue()->GetIntValue();
      operand.GetPtrValue()->SetIntValue(i + 1);
      return CreateResult<ReplValue>(std::move(operand));
    }
    else {
      uint64_t u = operand.GetPtrValue()->GetUIntValue();
      operand.GetPtrValue()->SetUIntValue(u + 1);
      return CreateResult<ReplValue>(std::move(operand));
    }
    break;
  case AST::UnaryExpr::UOP_SelfDecr:
    {
      int64_t i = operand.GetPtrValue()->GetIntValue();
      operand.GetPtrValue()->SetIntValue(i + 1);
      return CreateResult<ReplValue>(std::move(operand));
    }
    break;
  case AST::UnaryExpr::UOP_Deref:
    sona_unreachable1("not implemented");
    return CreateResult<>(VoidType());
    break;
  case AST::UnaryExpr::UOP_AddrOf:
    sona_unreachable1("not implemented");
    return CreateResult<>(VoidType());
    break;
  case AST::UnaryExpr::UOP_Positive:
    return CreateResult<ReplValue>(std::move(operand));
    break;
  case AST::UnaryExpr::UOP_Negative:
    if (exprOperandTy->IsSigned()) {
      int64_t i = operand.GetIntValue();
      return CreateResult<ReplValue>(ReplValue(-i));
    }
    else {
      double f = operand.GetFloatValue();
      return CreateResult<ReplValue>(ReplValue(-f));
    }
    break;
  case AST::UnaryExpr::UOP_BitReverse:
    {
      uint64_t u = operand.GetUIntValue();
      return CreateResult<ReplValue>(ReplValue(~u));
    }
  case AST::UnaryExpr::UOP_LogicNot:
    {
      bool b = operand.GetBoolValue();
      return CreateResult<ReplValue>(ReplValue(!b));
    }

  case AST::UnaryExpr::UOP_Invalid:
  default:
    sona_unreachable();
    return CreateResult<>(VoidType());
  }
}

sona::owner<ActionResult>
ReplInterpreter::VisitBinaryExpr(sona::ref_ptr<AST::BinaryExpr const> expr) {
  (void)expr;
  return CreateResult(VoidType());
}

sona::owner<ActionResult>
ReplInterpreter::VisitCondExpr(sona::ref_ptr<AST::CondExpr const> expr) {
  (void)expr;
  return CreateResult(VoidType());
}

sona::owner<ActionResult>
ReplInterpreter::VisitIdRefExpr(sona::ref_ptr<AST::IdRefExpr const> expr) {
  return CreateResult<ReplValue>(
        ReplValue(std::addressof(m_Values[expr->GetVarDecl()])));
}

sona::owner<ActionResult>
ReplInterpreter::VisitIntLiteralExpr(
    sona::ref_ptr<AST::IntLiteralExpr const> expr) {
  return CreateResult<ReplValue>(ReplValue(expr->GetValue()));
}

sona::owner<ActionResult>
ReplInterpreter::VisitUIntLiteralExpr(
    sona::ref_ptr<AST::UIntLiteralExpr const> expr) {
  return CreateResult<ReplValue>(ReplValue(expr->GetValue()));
}

sona::owner<ActionResult>
ReplInterpreter::VisitFloatLiteralExpr(
    sona::ref_ptr<AST::FloatLiteralExpr const> expr) {
  return CreateResult<ReplValue>(ReplValue(expr->GetValue()));
}

sona::owner<ActionResult>
ReplInterpreter::VisitCharLiteralExpr(
    sona::ref_ptr<AST::CharLiteralExpr const> expr) {
  return CreateResult<ReplValue>(ReplValue(expr->GetValue()));
}

sona::owner<ActionResult>
ReplInterpreter::VisitStringLiteralExpr(
    sona::ref_ptr<AST::StringLiteralExpr const>) {
  sona_unreachable1("not implemented");
  return CreateResult(VoidType());
}

sona::owner<ActionResult>
ReplInterpreter::VisitBoolLiteralExpr(
    sona::ref_ptr<AST::BoolLiteralExpr const> expr) {
  return CreateResult<ReplValue>(ReplValue(expr->GetValue()));
}

sona::owner<ActionResult>
ReplInterpreter::VisitNullptrLiteralExpr(
    sona::ref_ptr<AST::NullptrLiteralExpr const>) {
  sona_unreachable1("not implemented");
  return CreateResult(VoidType());
}

sona::owner<ActionResult>
ReplInterpreter::VisitParenExpr(sona::ref_ptr<AST::ParenExpr const> expr) {
  return expr->GetExpr()->Accept(this);
}

sona::owner<ActionResult>
ReplInterpreter::VisitImplicitCast(
    sona::ref_ptr<AST::ImplicitCast const> expr) {
  (void)expr;
  return CreateResult(VoidType());
}

sona::owner<ActionResult>
ReplInterpreter::VisitExplicitCastExpr(
    sona::ref_ptr<AST::ExplicitCastExpr const> expr) {
  (void)expr;
  return CreateResult(VoidType());
}

sona::owner<ActionResult>
ReplInterpreter::VisitTestExpr(sona::ref_ptr<AST::TestExpr const>) {
  sona_unreachable1("test expr cannot occur in repl context!");
  return CreateResult(VoidType());
}

void ReplInterpreter::DefineVar(sona::ref_ptr<const AST::VarDecl> decl) {
  m_Values.insert(std::make_pair(decl, ReplValue()));
}

} // namespace Backend
} // namespace ckx

