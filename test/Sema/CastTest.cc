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
      new AST::TestExpr(fromType, AST::Expr::VC_LValue);

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

void test2() {
  VkTestSectionStart("trivial nullptr -> pointer transformation");

  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;
  Diag::DiagnosticEngine diag("<undefined>", {});

  SemaPhase1Test semaTest(astContext, declContexts, diag);

  sona::owner<AST::Expr> castedExpr =
      new AST::NullptrLiteralExpr(
        astContext.GetBuiltinType(AST::BuiltinType::BTI_NilType));
  AST::QualType baseType =
      astContext.GetBuiltinType(AST::BuiltinType::BTI_Int8);
  baseType.AddConst();
  baseType.AddVolatile();
  AST::QualType destType = astContext.CreatePointerType(baseType);
  destType.AddConst();
  destType.AddVolatile();

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
  VkAssertEquals(1uL, implicitCast->GetCastSteps().size());
  VkAssertEquals(AST::CastStep::ICSK_Nil2Ptr,
                 implicitCast->GetCastSteps()[0].GetCSK());
}

void test3() {
  VkTestSectionStart("implicit pointer qualifier adjust");

  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;
  Diag::DiagnosticEngine diag("<undefined>", {});

  SemaPhase1Test semaTest(astContext, declContexts, diag);

  AST::QualType baseType =
      astContext.GetBuiltinType(AST::BuiltinType::BTI_Int8);
  AST::QualType baseType1 = baseType;
  baseType1.AddConst();
  baseType1.AddVolatile();
  AST::QualType fromType = astContext.CreatePointerType(baseType);
  AST::QualType destType = astContext.CreatePointerType(baseType1);
  destType.AddConst();

  sona::owner<AST::Expr> castedExpr =
      new AST::TestExpr(fromType, AST::Expr::VC_LValue);
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
  VkAssertEquals(AST::CastStep::CSK_AdjustPtrQual,
                 implicitCast->GetCastSteps()[1].GetCSK());
}

void test4() {
  VkTestSectionStart("explicit numeric cast 1");
  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;
  Diag::DiagnosticEngine diag("<undefined>", {});

  SemaPhase1Test semaTest(astContext, declContexts, diag);

  sona::owner<AST::Expr> castedExpr =
      new AST::IntLiteralExpr(
        12, astContext.GetBuiltinType(AST::BuiltinType::BTI_UInt8));
  AST::QualType destType =
      astContext.GetBuiltinType(AST::BuiltinType::BTI_Int32);

  sona::owner<AST::Expr> theCast =
      semaTest.ActOnStaticCast(nullptr, std::move(castedExpr), destType);
  VkAssertFalse(diag.HasPendingDiags());
  VkAssertNotEquals(nullptr, theCast.borrow());
  sona::ref_ptr<AST::ExplicitCastExpr> staticCast
      = theCast.borrow().cast_unsafe<AST::ExplicitCastExpr>();
  VkAssertEquals(destType, staticCast->GetExprType());
  VkAssertEquals(AST::Expr::VC_RValue, staticCast->GetValueCat());
  VkAssertEquals(1uL, staticCast->GetCastStepsUnsafe().size());
  VkAssertEquals(AST::CastStep::ECSK_Unsigned2Signed,
                 staticCast->GetCastStepsUnsafe()[0].GetCSK());
}

void test5() {
  VkTestSectionStart("explicit numeric cast 2");
  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;
  Diag::DiagnosticEngine diag("<undefined>", {});

  SemaPhase1Test semaTest(astContext, declContexts, diag);

  sona::owner<AST::Expr> castedExpr =
      new AST::FloatLiteralExpr(
        521.1314, astContext.GetBuiltinType(AST::BuiltinType::BTI_Double));
  AST::QualType destType =
      astContext.GetBuiltinType(AST::BuiltinType::BTI_UInt32);

  sona::owner<AST::Expr> theCast =
      semaTest.ActOnStaticCast(nullptr, std::move(castedExpr), destType);
  VkAssertFalse(diag.HasPendingDiags());
  VkAssertNotEquals(nullptr, theCast.borrow());
  sona::ref_ptr<AST::ExplicitCastExpr> staticCast
      = theCast.borrow().cast_unsafe<AST::ExplicitCastExpr>();
  VkAssertEquals(destType, staticCast->GetExprType());
  VkAssertEquals(AST::Expr::VC_RValue, staticCast->GetValueCat());
  VkAssertEquals(1uL, staticCast->GetCastStepsUnsafe().size());
  VkAssertEquals(AST::CastStep::ECSK_FLoat2UInt,
                 staticCast->GetCastStepsUnsafe()[0].GetCSK());
}

void test6() {
  VkTestSectionStart("explicit pointer qualifier adjust");
}

void test7() {
  VkTestSectionStart("explicit reference qualifier adjust");
}

int main() {
  VkTestStart();

  test0();
  test1();
  test2();
  test3();
  test4();
  test5();

  test6();
  test7();

  VkTestFinish();
}
