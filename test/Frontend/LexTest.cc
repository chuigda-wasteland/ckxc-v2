#include "VKTestCXX.h"
#include "Frontend/Lex.h"

#include "sona/log.h"
#include <string>

using namespace sona;
using namespace ckx;
using namespace std;

void test0() {
  VkTestSectionStart("Simple lexing test");

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
  VkTestSectionStart("Numeric literals and string literals test");

  string file = R"aacaac(33 12.5 1e8 1.2e8 "abc\nabc")aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);

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

void test2() {
  VkTestSectionStart("Char literals test");
  string file = R"np1p2('a' ' ' '\n' '\'')np1p2";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("b.c", lines);
  Frontend::Lexer lexer(move(file), diag);

  diag.EmitDiags();

  vector<Frontend::Token> tokens = lexer.GetAndReset();

  VkAssertFalse(diag.HasPendingDiags());
  diag.EmitDiags();

  VkAssertEquals(5uL, tokens.size());

  char chs[] = {'a', ' ', '\n', '\''};
  for (size_t i = 0; i < 4; i++) {
    VkAssertEquals(Frontend::Token::TK_LIT_CHAR, tokens[i].GetTokenKind());
    VkAssertEquals(chs[i], tokens[i].GetCharValueUnsafe());
  }
}

void test3() {
  VkTestSectionStart("Operators lexing test");

  string file = "";
  size_t operators = 0;
#define TOKEN_SYM(name, rep) { file += string(rep) + string(" "); operators++; }
#include "Frontend/Tokens.def"
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("c.c", lines);
  Frontend::Lexer lexer(move(file), diag);

  diag.EmitDiags();

  vector<Frontend::Token> tokens = lexer.GetAndReset();

  VkAssertFalse(diag.HasPendingDiags());

  Frontend::Token::TokenKind tks[] = {
#define TOKEN_SYM(name, rep) Frontend::Token::TK_SYM_##name,
#include "Frontend/Tokens.def"
  };

  VkAssertEquals(operators, tokens.size() - 1);
  for (size_t i = 0; i < operators; i++) {
    VkAssertEquals(tks[i], tokens[i].GetTokenKind());
  }
}

void test4() {
  VkTestSectionStart("Keywords lexing test");
  string file = "";
  size_t keywords = 0;
#define TOKEN_KWD(name, rep) { file += string(rep) + string(" "); keywords++; }
#include "Frontend/Tokens.def"
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("c.c", lines);
  Frontend::Lexer lexer(move(file), diag);

  diag.EmitDiags();

  vector<Frontend::Token> tokens = lexer.GetAndReset();

  VkAssertFalse(diag.HasPendingDiags());

  Frontend::Token::TokenKind tks[] = {
#define TOKEN_KWD(name, rep) Frontend::Token::TK_KW_##name,
#include "Frontend/Tokens.def"
  };

  VkAssertEquals(keywords, tokens.size() - 1);
  for (size_t i = 0; i < keywords; i++) {
    VkAssertEquals(tks[i], tokens[i].GetTokenKind());
  }
}

int main() {
  VkTestStart();

  test0();
  test1();
  test2();
  test3();
  test4();

  VkTestFinish();
}
