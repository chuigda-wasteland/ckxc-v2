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
  using Parser::ParseClassDecl;
  using Parser::ParseEnumDecl;
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

  VK_ASSERT_EQUALS(Syntax::Node::NodeKind::CNK_VarDecl,
                   decl.borrow()->GetNodeKind());

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

void test2() {
  VK_TEST_SECTION_BEGIN("Parsing class declaration");

  string file = R"aacaac( class A { def a : int32; def b : float; } )aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);

  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> decl = testContext.ParseClassDecl();
  VK_ASSERT_EQUALS(Syntax::Node::NodeKind::CNK_ClassDecl,
                   decl.borrow()->GetNodeKind());

  VK_ASSERT_NOT(diag.HasPendingDiags());
  VK_ASSERT_NOT_EQUALS(nullptr, decl.borrow());

  owner<Syntax::ClassDecl> classDecl = decl.cast_unsafe<Syntax::ClassDecl>();
  VK_ASSERT_EQUALS("A", classDecl.borrow()->GetClassName());
  VK_ASSERT_EQUALS(2, classDecl.borrow()->GetSubDecls().size());

  ref_ptr<Syntax::Decl const> d1 =
      *(classDecl.borrow()->GetSubDecls().begin());
  ref_ptr<Syntax::Decl const> d2 =
      *(classDecl.borrow()->GetSubDecls().begin() + 1);

  VK_ASSERT_EQUALS(Syntax::Node::NodeKind::CNK_VarDecl, d1->GetNodeKind());
  VK_ASSERT_EQUALS(Syntax::Node::NodeKind::CNK_VarDecl, d2->GetNodeKind());

  ref_ptr<Syntax::VarDecl const> varD1 =
      d1.cast_unsafe<Syntax::VarDecl const>();
  ref_ptr<Syntax::VarDecl const> varD2 =
      d2.cast_unsafe<Syntax::VarDecl const>();

  VK_ASSERT_EQUALS("a", varD1->GetName());
  VK_ASSERT_EQUALS("b", varD2->GetName());

  VK_TEST_SECTION_END("Parsing class declaration");
}

void test3() {
  VK_TEST_SECTION_BEGIN("Parsing enum declaration");

  string file = R"aacaac( enum A { a; b = 3; c; } )aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);
  std::vector<Frontend::Token> tokens = lexer.GetAndReset();
  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> decl = testContext.ParseEnumDecl();
  VK_ASSERT_EQUALS(Syntax::Node::NodeKind::CNK_EnumDecl,
                   decl.borrow()->GetNodeKind());

  VK_ASSERT_NOT(diag.HasPendingDiags());
  VK_ASSERT_NOT_EQUALS(nullptr, decl.borrow());

  owner<Syntax::EnumDecl> enumDecl = decl.cast_unsafe<Syntax::EnumDecl>();

  VK_ASSERT_EQUALS("A", enumDecl.borrow()->GetName());
  VK_ASSERT_EQUALS(3, enumDecl.borrow()->GetEnumerators().size());

  ref_ptr<Syntax::EnumDecl::Enumerator const> e1 =
      *(enumDecl.borrow()->GetEnumerators().begin());
  ref_ptr<Syntax::EnumDecl::Enumerator const> e2 =
      *(enumDecl.borrow()->GetEnumerators().begin());
  ref_ptr<Syntax::EnumDecl::Enumerator const> e3 =
      *(enumDecl.borrow()->GetEnumerators().begin());

  VK_ASSERT_NOT(e1->HasValue());
  VK_ASSERT(e2->HasValue());
  VK_ASSERT_EQUALS(3, e2->GetValueUnsafe());
  VK_ASSERT_NOT(e3->HasValue());

  VK_TEST_SECTION_END("Parsing enum declaration");
}

int main() {
  VK_TEST_BEGIN;

  test0();
  test1();
  test2();

  VK_TEST_END;
}
