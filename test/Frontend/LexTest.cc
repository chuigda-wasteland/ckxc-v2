#include "vktest.h"
#include "Frontend/Lex.h"

#include <string>

using namespace sona;
using namespace ckx;
using namespace std;

void test0() {
  VK_TEST_SECTION_BEGIN("Lexing test 1");

  string file = R"aacaac(def a, b : int16;)aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);

  vector<Frontend::Token> tokens = lexer.GetAndReset();

  VK_ASSERT_NOT(diag.HasPendingDiags());
  VK_ASSERT_EQUALS(8, tokens.size());
  VK_ASSERT_EQUALS(Frontend::Token::TK_KW_def, tokens[0].GetTokenKind());
  VK_ASSERT_EQUALS(Frontend::Token::TK_ID, tokens[1].GetTokenKind());
  VK_ASSERT_EQUALS("a", tokens[1].GetStrValueUnsafe());
  VK_ASSERT_EQUALS(Frontend::Token::TK_SYM_COMMA, tokens[2].GetTokenKind());
  VK_ASSERT_EQUALS(Frontend::Token::TK_ID, tokens[3].GetTokenKind());
  VK_ASSERT_EQUALS("b", tokens[3].GetStrValueUnsafe());
  VK_ASSERT_EQUALS(Frontend::Token::TK_SYM_COLON, tokens[4].GetTokenKind());
  VK_ASSERT_EQUALS(Frontend::Token::TK_KW_int16, tokens[5].GetTokenKind());
  VK_ASSERT_EQUALS(Frontend::Token::TK_SYM_SEMI, tokens[6].GetTokenKind());
  VK_ASSERT_EQUALS(Frontend::Token::TK_EOI, tokens[7].GetTokenKind());

  VK_TEST_SECTION_END("Lexing test 1");
}

void test1() {
  VK_TEST_SECTION_BEGIN("Lexing test 2");

  string file = R"aacaac(33 12.5 1e8 1.2e8 "abc\nabc")aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);

  diag.EmitDiags();

  vector<Frontend::Token> tokens = lexer.GetAndReset();

  VK_ASSERT_NOT(diag.HasPendingDiags());
  diag.EmitDiags();
  VK_ASSERT_EQUALS(6, tokens.size());
  VK_ASSERT_EQUALS(Frontend::Token::TK_LIT_INT, tokens[0].GetTokenKind());
  VK_ASSERT_EQUALS(33, tokens[0].GetIntValueUnsafe());
  VK_ASSERT_EQUALS(Frontend::Token::TK_LIT_FLOAT, tokens[1].GetTokenKind());
  VK_ASSERT_EQUALS_F(12.5, tokens[1].GetFloatValueUnsafe());
  VK_ASSERT_EQUALS(Frontend::Token::TK_LIT_FLOAT, tokens[2].GetTokenKind());
  VK_ASSERT_EQUALS_F(1e8, tokens[2].GetFloatValueUnsafe());
  VK_ASSERT_EQUALS(Frontend::Token::TK_LIT_FLOAT, tokens[3].GetTokenKind());
  VK_ASSERT_EQUALS_F(1.2e8, tokens[3].GetFloatValueUnsafe());
  VK_ASSERT_EQUALS(Frontend::Token::TK_LIT_STR, tokens[4].GetTokenKind());
  VK_ASSERT_EQUALS("abc\nabc", tokens[4].GetStrValueUnsafe());
  VK_ASSERT_EQUALS(Frontend::Token::TK_EOI, tokens[5].GetTokenKind());

  VK_TEST_SECTION_END("Lexing test 2");
}

int main() {
  VK_TEST_BEGIN;

  test0();
  test1();

  VK_TEST_END;
  getchar();
}
