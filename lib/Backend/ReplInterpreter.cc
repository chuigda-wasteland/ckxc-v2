#include "Backend/ReplInterpreter.h"

#include "sona/util.h"

namespace ckx {
namespace Backend {

sona::owner<ActionResult>
VisitAssignExpr(sona::ref_ptr<AST::AssignExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitUnaryExpr(sona::ref_ptr<AST::UnaryExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitBinaryExpr(sona::ref_ptr<AST::BinaryExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitCondExpr(sona::ref_ptr<AST::CondExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitIdRefExpr(sona::ref_ptr<AST::IdRefExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitIntLiteralExpr(sona::ref_ptr<AST::IntLiteralExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitUIntLiteralExpr(sona::ref_ptr<AST::UIntLiteralExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitFloatLiteralExpr(sona::ref_ptr<AST::FloatLiteralExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitCharLiteralExpr(sona::ref_ptr<AST::CharLiteralExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitStringLiteralExpr(sona::ref_ptr<AST::StringLiteralExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitBoolLiteralExpr(sona::ref_ptr<AST::BoolLiteralExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitNullptrLiteralExpr(sona::ref_ptr<AST::NullptrLiteralExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitParenExpr(sona::ref_ptr<AST::ParenExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitImplicitCast(sona::ref_ptr<AST::ImplicitCast const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitExplicitCastExpr(sona::ref_ptr<AST::ExplicitCastExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

sona::owner<ActionResult>
VisitTestExpr(sona::ref_ptr<AST::TestExpr const> expr) {
  (void)expr; return CreateResult(VoidType());
}

void ReplInterpreter::DefineVar(sona::ref_ptr<const AST::VarDecl> decl) {
  m_Values.insert(std::make_pair(decl, ReplValue()));
}

} // namespace Backend
} // namespace ckx

