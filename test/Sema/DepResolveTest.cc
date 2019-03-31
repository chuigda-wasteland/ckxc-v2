#include "VKTestCXX.hpp"
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
  SemaPhase0Test(Diag::DiagnosticEngine &diag) : SemaPhase0(diag) {}

  std::unordered_map<sona::ref_ptr<AST::VarDecl const>,
                     Sema::IncompleteVarDecl> const&
  GetIncompleteVars() const noexcept { return m_IncompleteVars; }

  std::unordered_map<sona::ref_ptr<AST::Decl const>,
                     Sema::IncompleteTagDecl> const&
  GetIncompleteTags() const noexcept { return m_IncompleteTags; }

  std::unordered_map<sona::ref_ptr<AST::UsingDecl const>,
                     Sema::IncompleteUsingDecl> const&
  GetIncompleteUsings() const noexcept { return m_IncompleteUsings; }

  std::unordered_map<sona::ref_ptr<AST::EnumClassInternDecl const>,
                     Sema::IncompleteEnumClassInternDecl> const&
  GetIncompleteEnumClassInterns() const noexcept {
    return m_IncompleteEnumClassInterns;
  }
};

void test0() {
  VkTestSectionStart("DepResolve");

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

  SemaPhase0Test sema0(diag);
  sona::owner<AST::TransUnitDecl> transUnit =
      sema0.ActOnTransUnit(cst.borrow());

  VkAssertEquals(2uL, sema0.GetIncompleteTags().size());
  VkAssertEquals(2uL, sema0.GetIncompleteVars().size());

  for (const auto &incompleteTagPair : sema0.GetIncompleteTags()) {
    VkAssertEquals(AST::Decl::DeclKind::DK_Class,
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

  return;
}

int main() {
  VkTestStart();

  test0();

  VkTestFinish();
}
