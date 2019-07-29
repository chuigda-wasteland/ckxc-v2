#include "VKTestCXX.h"
#include "Sema/SemaPhase1.h"

using namespace sona;
using namespace ckx;
using namespace std;

class SemaPhase1Test : public Sema::SemaPhase1 {
public:
  using SemaPhase1::TryImplicitCast;
  using SemaPhase1::ActOnStaticCast;
  using SemaPhase1::ActOnConstCast;

  SemaPhase1Test(AST::ASTContext &astContext,
                 std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
                 Diag::DiagnosticEngine &diag)
    : SemaPhase1(astContext, declContexts, diag) {}
};

void test0() {
  VkTestSectionStart("implicit numeric promotion");

  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;
  Diag::DiagnosticEngine diag("<undefined>", {});

  SemaPhase1Test semaTest(astContext, declContexts, diag);

  sona::owner<AST::Expr> castedExpr =
      new AST::IntLiteralExpr(
        12, astContext.GetBuiltinType(AST::BuiltinType::BTI_Int8));
  AST::QualType destType =
      astContext.GetBuiltinType(AST::BuiltinType::BTI_Int32);

  sona::owner<AST::Expr> theCast =
      semaTest.TryImplicitCast(nullptr, std::move(castedExpr), destType, true);

  VkAssertFalse(diag.HasPendingDiags());
  VkAssertNotEquals(nullptr, theCast.borrow());
  VkAssertEquals(AST::Expr::ExprId::EI_ImplicitCast,
                 theCast.borrow()->GetExprId());
  VkAssertEquals(destType, theCast.borrow()->GetExprType());
  VkAssertEquals(AST::Expr::VC_RValue, theCast.borrow()->GetValueCat());
}

void test1() {
  VkTestSectionStart("implicit numeric promotion with L2R casting");

  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;
  Diag::DiagnosticEngine diag("<undefined>", {});

  SemaPhase1Test semaTest(astContext, declContexts, diag);

  AST::QualType fromType =
      astContext.GetBuiltinType(AST::BuiltinType::BTI_UInt8);
  fromType.AddConst();
  AST::QualType destType =
      astContext.GetBuiltinType(AST::BuiltinType::BTI_UInt32);
  destType.AddVolatile();

  sona::owner<AST::Expr> castedExpr =
      new AST::IdExpr("fnmdp", fromType, AST::Expr::VC_LValue);

  sona::owner<AST::Expr> theCast =
      semaTest.TryImplicitCast(nullptr, std::move(castedExpr), destType, true);

  VkAssertFalse(diag.HasPendingDiags());
  VkAssertNotEquals(nullptr, theCast.borrow());
  VkAssertEquals(AST::Expr::ExprId::EI_ImplicitCast,
                 theCast.borrow()->GetExprId());
  sona::ref_ptr<AST::ImplicitCast> implicitCast =
      theCast.borrow().cast_unsafe<AST::ImplicitCast>();
  VkAssertEquals(destType.DeQual(), implicitCast->GetExprType());
  VkAssertEquals(AST::Expr::VC_RValue, implicitCast->GetValueCat());
  VkAssertEquals(2uL, implicitCast->GetCastSteps().size());
  VkAssertEquals(AST::CastStep::ICSK_LValue2RValue,
                 implicitCast->GetCastSteps()[0].GetCSK());
  VkAssertEquals(AST::CastStep::ICSK_UIntPromote,
                 implicitCast->GetCastSteps()[1].GetCSK());
}

int main() {
  VkTestStart();

  test0();
  test1();

  VkTestFinish();
}
