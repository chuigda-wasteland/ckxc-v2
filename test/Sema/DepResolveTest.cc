#include "VKTestCXX.hpp"
#include "Frontend/Lex.h"
#include "Frontend/Parser.h"
#include "Sema/SemaPhase0.h"

#include <iostream>
#include <string>

using namespace sona;
using namespace ckx;
using namespace std;

void test0() {
  VkTestSectionStart("DepResolve");

  string f0 = R"aacaac(class A { def b : B; })aacaac";
  string f1 = R"aacaac(class B { def i : int32; })aacaac";
  string f2 = R"aacaac(class C { def a : A; })aacaac";

  string file = f0 + "\n" + f1 + "\n" + f2;

  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  Frontend::Parser parser(diag);
  sona::owner<Syntax::TransUnit> cst = parser.ParseTransUnit(tokens);
  Sema::SemaPhase0 sema0(diag);

  sema0.ActOnTransUnit(cst.borrow());

  return;
}

int main() {
  VkTestStart();

  test0();

  VkTestFinish();
}
