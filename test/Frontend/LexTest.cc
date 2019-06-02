#include "VKTestCXX.h"
#include "Frontend/Lex.h"

#include <string>

using namespace sona;
using namespace ckx;
using namespace std;

void test0() {
  VkTestSectionStart("Lexing test 1");

  string file = R"aacaac(def a, nmsl!? : int16;)aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);

  vector<Frontend::Token> tokens = lexer.GetAndReset();

  VkAssertFalse(diag.HasPendingDiags());
  VkAssertEquals(8uL, tokens.size());
  VkAssertEquals(Frontend::Token::TK_KW_def, tokens[0].GetTokenKind());
  VkAssertEquals(Frontend::Token::TK_ID, tokens[1].GetTokenKind());
  VkAssertEquals("a", tokens[1].GetStrValueUnsafe());
  VkAssertEquals(Frontend::Token::TK_SYM_COMMA, tokens[2].GetTokenKind());
  VkAssertEquals(Frontend::Token::TK_ID, tokens[3].GetTokenKind());
  VkAssertEquals("nmsl!?", tokens[3].GetStrValueUnsafe());
  VkAssertEquals(Frontend::Token::TK_SYM_COLON, tokens[4].GetTokenKind());
  VkAssertEquals(Frontend::Token::TK_KW_int16, tokens[5].GetTokenKind());
  VkAssertEquals(Frontend::Token::TK_SYM_SEMI, tokens[6].GetTokenKind());
  VkAssertEquals(Frontend::Token::TK_EOI, tokens[7].GetTokenKind());
}

void test1() {
  VkTestSectionStart("Lexing test 2");

  string file = R"aacaac(33 12.5 1e8 1.2e8 "abc\nabc")aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);

  diag.EmitDiags();

  vector<Frontend::Token> tokens = lexer.GetAndReset();

  VkAssertFalse(diag.HasPendingDiags());
  diag.EmitDiags();
  VkAssertEquals(6uL, tokens.size());
  VkAssertEquals(Frontend::Token::TK_LIT_INT, tokens[0].GetTokenKind());
  VkAssertEquals(33, tokens[0].GetIntValueUnsafe());
  VkAssertEquals(Frontend::Token::TK_LIT_FLOAT, tokens[1].GetTokenKind());
  VkAssertEquals(12.5, tokens[1].GetFloatValueUnsafe());
  VkAssertEquals(Frontend::Token::TK_LIT_FLOAT, tokens[2].GetTokenKind());
  VkAssertEquals(1e8, tokens[2].GetFloatValueUnsafe());
  VkAssertEquals(Frontend::Token::TK_LIT_FLOAT, tokens[3].GetTokenKind());
  VkAssertEquals(1.2e8, tokens[3].GetFloatValueUnsafe());
  VkAssertEquals(Frontend::Token::TK_LIT_STR, tokens[4].GetTokenKind());
  VkAssertEquals("abc\nabc", tokens[4].GetStrValueUnsafe());
  VkAssertEquals(Frontend::Token::TK_EOI, tokens[5].GetTokenKind());
}

int main() {
  VkTestStart();

  test0();
  test1();

  VkTestFinish();
}
