#include "vktest.h"
#include "Frontend/Lex.h"
#include "Frontend/Parser.h"

#include <iostream>
#include <string>

using namespace sona;
using namespace ckx;
using namespace std;

class ParserTest : public Frontend::Parser {
public:
  ParserTest(Diag::DiagnosticEngine &diag) : Parser(diag) {}

  using Parser::SetParsingTokenStream;
  using Parser::ParseVarDecl;
  using Parser::ParseFuncDecl;
};

void test0() {
  VK_TEST_SECTION_BEGIN("Parsing variable declaration");

  string file = R"aacaac(def a : int16;)aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);

  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> decl = testContext.ParseVarDecl();

  sona_assert(decl.borrow()->GetNodeKind() ==
              Syntax::Node::NodeKind::CNK_VarDecl);

  ref_ptr<Syntax::VarDecl> varDecl =
      decl.borrow().cast_unsafe<Syntax::VarDecl>();
  VK_ASSERT_NOT(diag.HasPendingDiags());
  VK_ASSERT_EQUALS("a", varDecl->GetName());

  diag.EmitDiags();

  VK_TEST_SECTION_END("Parsing variable declaration");
}

void test1() {
  VK_TEST_SECTION_BEGIN("Parsing function declaration");

  string file = R"aacaac(func f(a : int16, b : uint16) : int16;)aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);

  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> decl = testContext.ParseFuncDecl();
  VK_ASSERT_NOT_EQUALS(nullptr, decl.borrow());
  VK_ASSERT_NOT(diag.HasPendingDiags());

  diag.EmitDiags();

  owner<Syntax::FuncDecl> funcDecl = decl.cast_unsafe<Syntax::FuncDecl>();
  VK_ASSERT_EQUALS("f", funcDecl.borrow()->GetName());
  VK_ASSERT_EQUALS(2, funcDecl.borrow()->GetParamNames().size());
  VK_ASSERT_EQUALS("a", funcDecl.borrow()->GetParamNames()[0]);
  VK_ASSERT_EQUALS("b", funcDecl.borrow()->GetParamNames()[1]);

  VK_TEST_SECTION_END("Parsing function declaration")
}

int main() {
  VK_TEST_BEGIN;

  test0();
  test1();

  VK_TEST_END;
}
