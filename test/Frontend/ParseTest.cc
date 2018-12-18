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

  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> decl = testContext.ParseVarDecl();

  sona_assert(decl.borrow()->GetNodeKind() ==
              Syntax::Node::NodeKind::CNK_VarDecl);
  /// @todo add some casting assist
  ref_ptr<Syntax::VarDecl> varDecl =
      decl.borrow().cast_unsafe<Syntax::VarDecl>();
  VK_ASSERT_NOT(diag.HasPendingDiags());
  VK_ASSERT_EQUALS("a", varDecl->GetName());

  VK_TEST_SECTION_END("Parsing test 1");
}

int main() {
  VK_TEST_BEGIN;

  test0();

  VK_TEST_END;
}
