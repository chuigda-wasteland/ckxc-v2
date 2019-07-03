#include "VKTestCXX.h"
#include "Frontend/Lex.h"
#include "Frontend/Parser.h"
#include "Sema/SemaPhase0.h"

#include <iostream>
#include <string>

using namespace sona;
using namespace ckx;
using namespace std;

class SemaPhase0Test : public Sema::SemaPhase0 {
public:
  SemaPhase0Test(AST::ASTContext &astContext,
                 std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
                 Diag::DiagnosticEngine &diag)
    : SemaPhase0(astContext, declContexts, diag) {}

  std::unordered_map<sona::ref_ptr<AST::VarDecl const>,
                     Sema::IncompleteVarDecl> const&
  GetIncompleteVars() const noexcept { return m_IncompleteVars; }

  std::unordered_map<sona::ref_ptr<AST::Decl const>,
                     Sema::IncompleteTagDecl> const&
  GetIncompleteTags() const noexcept { return m_IncompleteTags; }

  std::unordered_map<sona::ref_ptr<AST::UsingDecl const>,
                     Sema::IncompleteUsingDecl> const&
  GetIncompleteUsings() const noexcept { return m_IncompleteUsings; }

  std::unordered_map<sona::ref_ptr<AST::ValueCtorDecl const>,
                     Sema::IncompleteValueCtorDecl> const&
  GetIncompleteValueCtors() const noexcept {
    return m_IncompleteValueCtors;
  }

  std::vector<Sema::IncompleteFuncDecl> const&
  GetIncompleteFuncs() const noexcept {
    return m_IncompleteFuncs;
  }
};

void test0() {
  VkTestSectionStart("Basic dependency resolve");

  string f0 = R"aacaac(class A { def b : B; })aacaac";
  string f1 = R"aacaac(class B { def i : int32; })aacaac";
  string f2 = R"aacaac(class C { def a : A; })aacaac";

  string file = f0 + "\n" + f1 + "\n" + f2;

  vector<string> lines = { f0, f1, f2 };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  Frontend::Parser parser(diag);
  sona::owner<Syntax::TransUnit> cst = parser.ParseTransUnit(tokens);

  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;

  SemaPhase0Test sema0(astContext, declContexts, diag);

  sona::owner<AST::TransUnitDecl> transUnit =
      sema0.ActOnTransUnit(cst.borrow());

  VkAssertFalse(diag.HasPendingDiags());

  diag.EmitDiags();

  VkAssertEquals(2uL, sema0.GetIncompleteTags().size());
  VkAssertEquals(2uL, sema0.GetIncompleteVars().size());
  VkAssertEquals(0uL, sema0.GetIncompleteUsings().size());
  VkAssertEquals(0uL, sema0.GetIncompleteValueCtors().size());

  for (const auto &incompleteTagPair : sema0.GetIncompleteTags()) {
    VkAssertEquals(AST::Decl::DK_Class,
                   incompleteTagPair.first->GetDeclKind());
    VkAssertEquals(1uL, incompleteTagPair.second.GetDependencies().size());
    if (incompleteTagPair.first.cast_unsafe<AST::ClassDecl const>()->GetName()
        == "A") {
      ref_ptr<AST::Decl const> dvar =
          incompleteTagPair.second.GetDependencies().front().GetDeclUnsafe();
      VkAssertEquals("b", dvar.cast_unsafe<AST::VarDecl const>()->GetVarName());
    }
    else {
      VkAssertEquals("C", incompleteTagPair.first
                          .cast_unsafe<AST::ClassDecl const>()->GetName());
      ref_ptr<AST::Decl const> dvar =
          incompleteTagPair.second.GetDependencies().front().GetDeclUnsafe();
      VkAssertEquals("a", dvar.cast_unsafe<AST::VarDecl const>()->GetVarName());
    }
  }

  for (const auto &incompleteVarPair : sema0.GetIncompleteVars()) {
    VkAssertEquals(1uL, incompleteVarPair.second.GetDependencies().size());
    if (incompleteVarPair.first->GetVarName() == "a") {
      ref_ptr<AST::ClassDecl const> dclass =
          incompleteVarPair.second.GetDependencies().front().GetDeclUnsafe()
                           .cast_unsafe<AST::ClassDecl const>();
      VkAssertEquals("A", dclass->GetName());
    }
    else {
      VkAssertEquals("b", incompleteVarPair.first->GetVarName());
      VkAssertEquals("B", incompleteVarPair.second.GetDependencies().front()
                                           .GetIdUnsafe().GetIdentifier());
    }
  }

  return;
}

void test1() {
  VkTestSectionStart("Resolving with enums");

  string f0 = R"aacaac(enum N { a = 3; b; c; })aacaac";
  string f1 = R"aacaac(class A { def n : N; })aacaac";
  string f2 = R"aacaac(class B { def a : A; def c : C; })aacaac";
  string f3 = R"996icu(class C { def a : A; def n : N; })996icu";

  string file = f0 + "\n" + f1 + "\n" + f2 + "\n" + f3;

  vector<string> lines = { f0, f1, f2, f3 };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  Frontend::Parser parser(diag);
  sona::owner<Syntax::TransUnit> cst = parser.ParseTransUnit(tokens);

  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;

  SemaPhase0Test sema0(astContext, declContexts, diag);
  sona::owner<AST::TransUnitDecl> transUnit =
      sema0.ActOnTransUnit(cst.borrow());

  VkAssertFalse(diag.HasPendingDiags());

  VkAssertEquals(0uL, sema0.GetIncompleteValueCtors().size());
  VkAssertEquals(0uL, sema0.GetIncompleteUsings().size());
  VkAssertEquals(0uL, sema0.GetIncompleteFuncs().size());
  VkAssertEquals(1uL, sema0.GetIncompleteTags().size());
  VkAssertEquals(1uL, sema0.GetIncompleteVars().size());

  const auto& incompleteTagPair = *sema0.GetIncompleteTags().begin();
  const auto& incompleteVarPair = *sema0.GetIncompleteVars().begin();

  VkAssertEquals(1uL, incompleteTagPair.second.GetDependencies().size());
  VkAssertEquals("B", incompleteTagPair.first
                                       .cast_unsafe<AST::ClassDecl const>()
                                       ->GetName());

  VkAssertEquals(1uL, incompleteVarPair.second.GetDependencies().size());
  VkAssertEquals("c", incompleteVarPair.first->GetVarName());
}

void test2() {
  VkTestSectionStart("Resolving ADTs");

  string f0 = R"aacaac(class A { def c : C; })aacaac";
  string f1 = R"aacaac(enum class B { Cc1(A); Cc2(C); })aacaac";
  string f2 = R"aacaac(class C { def i : int32; })aacaac";

  string file = f0 + "\n" + f1 + "\n" + f2;

  vector<string> lines = { f0, f1, f2 };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  Frontend::Parser parser(diag);
  sona::owner<Syntax::TransUnit> cst = parser.ParseTransUnit(tokens);

  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;

  SemaPhase0Test sema0(astContext, declContexts, diag);

  sona::owner<AST::TransUnitDecl> transUnit =
      sema0.ActOnTransUnit(cst.borrow());

  VkAssertFalse(diag.HasPendingDiags());
  VkAssertEquals(2uL, sema0.GetIncompleteValueCtors().size());
  VkAssertEquals(0uL, sema0.GetIncompleteUsings().size());
  VkAssertEquals(0uL, sema0.GetIncompleteFuncs().size());
  VkAssertEquals(2uL, sema0.GetIncompleteTags().size());
  VkAssertEquals(1uL, sema0.GetIncompleteVars().size());

  for (const auto &incompleteTagPair : sema0.GetIncompleteTags()) {
    if (incompleteTagPair.first->GetDeclKind() == AST::Decl::DK_ADT) {
      sona::ref_ptr<AST::ADTDecl const> adt =
          incompleteTagPair.first.cast_unsafe<AST::ADTDecl const>();
      VkAssertEquals("B", adt->GetName());
      VkAssertEquals(2uL, incompleteTagPair.second.GetDependencies().size());
      for (const auto& dep : incompleteTagPair.second.GetDependencies()) {
        VkAssertTrue(dep.IsStrong());
        VkAssertFalse(dep.IsDependByname());
        VkAssertEquals(nullptr,
                       dep.GetDeclUnsafe()
                          .cast_unsafe<AST::ValueCtorDecl const>()
                          ->GetType().GetUnqualTy());
      }
    }
  }

  for (const auto &incompleteDataPair : sema0.GetIncompleteValueCtors()) {
    VkAssertEquals(1uL, incompleteDataPair.second.GetDependencies().size());
    if (incompleteDataPair.first->GetConstructorName() == "Cc1") {
      sona::ref_ptr<AST::ClassDecl const> dependingClass =
          incompleteDataPair.second.GetDependencies().front().GetDeclUnsafe()
                            .cast_unsafe<AST::ClassDecl const>();
      VkAssertEquals("A", dependingClass->GetName());
    }
    else {
      VkAssertEquals("Cc2", incompleteDataPair.first->GetConstructorName());
      VkAssertEquals("C", incompleteDataPair.second.GetDependencies().front()
                                            .GetIdUnsafe().GetIdentifier());
    }
  }
}

void test3() {
  VkTestSectionStart("Resolve using declarations");

  string f0 = R"aacaac(class A { def b: B; })aacaac";
  string f1 = R"aacaac(using RB = B;)aacaac";
  string f2 = R"aacaac(using RA = A;)aacaac";
  string f3 = R"996icu(class B { def i : int32; })996icu";

  string file = f0 + "\n" + f1 + "\n" + f2 + "\n" + f3;

  vector<string> lines = { f0, f1, f2, f3 };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  Frontend::Parser parser(diag);
  sona::owner<Syntax::TransUnit> cst = parser.ParseTransUnit(tokens);

  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;

  SemaPhase0Test sema0(astContext, declContexts, diag);

  sona::owner<AST::TransUnitDecl> transUnit =
      sema0.ActOnTransUnit(cst.borrow());

  VkAssertFalse(diag.HasPendingDiags());
  diag.EmitDiags();
  VkAssertEquals(0uL, sema0.GetIncompleteValueCtors().size());
  VkAssertEquals(2uL, sema0.GetIncompleteUsings().size());
  VkAssertEquals(0uL, sema0.GetIncompleteFuncs().size());
  VkAssertEquals(1uL, sema0.GetIncompleteTags().size());
  VkAssertEquals(1uL, sema0.GetIncompleteVars().size());

  for (const auto& incompleteUsingPair : sema0.GetIncompleteUsings()) {
    if (incompleteUsingPair.first->GetName() == "RA") {
      VkAssertEquals(1uL, incompleteUsingPair.second.GetDependencies().size());
      VkAssertTrue(incompleteUsingPair.second.GetDependencies()
                                             .front().IsStrong());
      VkAssertFalse(incompleteUsingPair.second.GetDependencies()
                                              .front().IsDependByname());
      VkAssertEquals("A", incompleteUsingPair.second
                              .GetDependencies()
                              .front().GetDeclUnsafe()
                              .cast_unsafe<AST::ClassDecl const>()
                              ->GetName());
    }
    else {
      VkAssertEquals("RB", incompleteUsingPair.first->GetName());
      VkAssertTrue(incompleteUsingPair.second.GetDependencies()
                                             .front().IsStrong());
      VkAssertTrue(incompleteUsingPair.second.GetDependencies()
                                             .front().IsDependByname());
      VkAssertEquals("B", incompleteUsingPair.second.GetDependencies()
                                             .front().GetIdUnsafe()
                                             .GetIdentifier());
    }
  }
}

int main() {
  VkTestStart();

  test0();
  test1();
  test2();
  test3();

  VkTestFinish();
}
