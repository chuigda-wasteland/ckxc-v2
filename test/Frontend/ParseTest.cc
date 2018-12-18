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
};

void test0() {
  VK_TEST_SECTION_BEGIN("Parsing test 1");

  string file = R"aacaac(def a : int16;)aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);

  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  cerr << "Lexing success" << endl;

  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> varDecl = testContext.ParseVarDecl();

  cerr << "Parsing success" << endl;

  diag.EmitDiags();

  VK_TEST_SECTION_END("Parsing test 1");
}

int main() {
  VK_TEST_BEGIN;

  test0();

  VK_TEST_END;
}
