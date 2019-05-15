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
  SemaPhase0Test(AST::ASTContext &astContext,
                 std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
                 std::vector<std::shared_ptr<Sema::Scope>> &scopeChains,
                 Diag::DiagnosticEngine &diag)
    : SemaPhase0(astContext, declContexts, scopeChains, diag) {}

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

  std::vector<Sema::IncompleteFuncDecl> const&
  GetIncompleteFuncs() const noexcept {
    return m_IncompleteFuncs;
  }

  using SemaPhase0::PostSubstituteDepends;
};

void test0() {
  VkTestSectionStart("Basic dependency resolve: post substitute dependencies");

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
  std::vector<std::shared_ptr<Sema::Scope>> scopeChains;

  SemaPhase0Test sema0(astContext, declContexts, scopeChains, diag);
  sona::owner<AST::TransUnitDecl> transUnit =
      sema0.ActOnTransUnit(cst.borrow());
  sema0.PostSubstituteDepends();

  VkAssertFalse(diag.HasPendingDiags());

  VkAssertEquals(2uL, sema0.GetIncompleteTags().size());
  VkAssertEquals(2uL, sema0.GetIncompleteVars().size());
  VkAssertEquals(0uL, sema0.GetIncompleteUsings().size());
  VkAssertEquals(0uL, sema0.GetIncompleteEnumClassInterns().size());

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
      ref_ptr<AST::ClassDecl const> dclass =
          incompleteVarPair.second.GetDependencies().front().GetDeclUnsafe()
                           .cast_unsafe<AST::ClassDecl const>();
      VkAssertEquals("B", dclass->GetName());
    }
  }

  std::vector<sona::ref_ptr<Sema::IncompleteDecl>> transOrder =
      sema0.FindTranslationOrder();
  for (sona::ref_ptr<Sema::IncompleteDecl> d : transOrder) {
    std::cerr << d->ToString() << std::endl;
  }
}

int main() {
  VkTestStart();

  test0();

  VkTestFinish();
}
