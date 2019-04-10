#include "VKTestCXX.hpp"
#include "Frontend/Lex.h"
#include "Frontend/ParserImpl.h"

#include <iostream>
#include <string>

using namespace sona;
using namespace ckx;
using namespace std;

class ParserTest : public Frontend::ParserImpl {
public:
  ParserTest(Diag::DiagnosticEngine &diag) : ParserImpl(diag) {}

  using ParserImpl::SetParsingTokenStream;
  using ParserImpl::ParseVarDecl;
  using ParserImpl::ParseClassDecl;
  using ParserImpl::ParseEnumDecl;
  using ParserImpl::ParseUsingDecl;
  using ParserImpl::ParseFuncDecl;
  using ParserImpl::ParseLiteralExpr;
};

void test0() {
  VkTestSectionStart("Parsing variable declaration");

  string file = R"aacaac(def a : int16;)aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);

  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> decl = testContext.ParseVarDecl();

  VkAssertEquals(Syntax::Node::NodeKind::CNK_VarDecl,
                   decl.borrow()->GetNodeKind());

  ref_ptr<Syntax::VarDecl> varDecl =
      decl.borrow().cast_unsafe<Syntax::VarDecl>();
  VkAssertFalse(diag.HasPendingDiags());
  VkAssertEquals("a", varDecl->GetName());

  diag.EmitDiags();
}

void test1() {
  VkTestSectionStart("Parsing function declaration");

  string file = R"aacaac(func f!(a : int16, b : uint16) : int16;)aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);

  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> decl = testContext.ParseFuncDecl();
  VkAssertNotEquals(nullptr, decl.borrow());
  VkAssertFalse(diag.HasPendingDiags());

  diag.EmitDiags();

  owner<Syntax::FuncDecl> funcDecl = decl.cast_unsafe<Syntax::FuncDecl>();
  VkAssertEquals("f!", funcDecl.borrow()->GetName());
  VkAssertEquals(2uL, funcDecl.borrow()->GetParamNames().size());
  VkAssertEquals("a", funcDecl.borrow()->GetParamNames()[0]);
  VkAssertEquals("b", funcDecl.borrow()->GetParamNames()[1]);
}

void test2() {
  VkTestSectionStart("Parsing class declaration");

  string file = R"aacaac( class A { def a : int32; def b : float; } )aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);

  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> decl = testContext.ParseClassDecl();
  VkAssertEquals(Syntax::Node::NodeKind::CNK_ClassDecl,
                   decl.borrow()->GetNodeKind());

  VkAssertFalse(diag.HasPendingDiags());
  VkAssertNotEquals(nullptr, decl.borrow());

  owner<Syntax::ClassDecl> classDecl = decl.cast_unsafe<Syntax::ClassDecl>();
  VkAssertEquals("A", classDecl.borrow()->GetClassName());
  VkAssertEquals(2uL, classDecl.borrow()->GetSubDecls().size());

  ref_ptr<Syntax::Decl const> d1 =
      *(classDecl.borrow()->GetSubDecls().begin());
  ref_ptr<Syntax::Decl const> d2 =
      *(classDecl.borrow()->GetSubDecls().begin() + 1);

  VkAssertEquals(Syntax::Node::NodeKind::CNK_VarDecl, d1->GetNodeKind());
  VkAssertEquals(Syntax::Node::NodeKind::CNK_VarDecl, d2->GetNodeKind());

  ref_ptr<Syntax::VarDecl const> varD1 =
      d1.cast_unsafe<Syntax::VarDecl const>();
  ref_ptr<Syntax::VarDecl const> varD2 =
      d2.cast_unsafe<Syntax::VarDecl const>();

  VkAssertEquals("a", varD1->GetName());
  VkAssertEquals("b", varD2->GetName());
}

void test3() {
  VkTestSectionStart("Parsing enum declaration");

  string file = R"aacaac( enum A { a; b = 3; c; } )aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);
  std::vector<Frontend::Token> tokens = lexer.GetAndReset();
  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> decl = testContext.ParseEnumDecl();
  VkAssertEquals(Syntax::Node::NodeKind::CNK_EnumDecl,
                   decl.borrow()->GetNodeKind());

  VkAssertFalse(diag.HasPendingDiags());
  VkAssertNotEquals(nullptr, decl.borrow());

  diag.EmitDiags();

  owner<Syntax::EnumDecl> enumDecl = decl.cast_unsafe<Syntax::EnumDecl>();

  VkAssertEquals("A", enumDecl.borrow()->GetName());
  VkAssertEquals(3uL, enumDecl.borrow()->GetEnumerators().size());

  ref_ptr<Syntax::EnumDecl::Enumerator const> e1 =
      *(enumDecl.borrow()->GetEnumerators().begin());
  ref_ptr<Syntax::EnumDecl::Enumerator const> e2 =
      *(enumDecl.borrow()->GetEnumerators().begin() + 1);
  ref_ptr<Syntax::EnumDecl::Enumerator const> e3 =
      *(enumDecl.borrow()->GetEnumerators().begin() + 2);

  VkAssertFalse(e1->HasValue());
  VkAssertTrue(e2->HasValue());
  VkAssertEquals(3L, e2->GetValueUnsafe());
  VkAssertFalse(e3->HasValue());
}

void test4() {
  VkTestSectionStart("Parsing int literal expr");

  string file = R"aacaac( 123123 )aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);
  std::vector<Frontend::Token> tokens = lexer.GetAndReset();
  testContext.SetParsingTokenStream(tokens);
  sona::owner<Syntax::Expr> e = testContext.ParseLiteralExpr();

  VkAssertEquals(Syntax::Node::NodeKind::CNK_LiteralExpr,
                   e.borrow()->GetNodeKind());
  VkAssertFalse(diag.HasPendingDiags());

  diag.EmitDiags();

  sona::owner<Syntax::LiteralExpr> literalExpr =
      e.cast_unsafe<Syntax::LiteralExpr>();

  VkAssertEquals(Syntax::BasicType::TypeKind::TK_Int32,
                 literalExpr.borrow()->GetLiteralTypeKind());
  VkAssertEquals(123123,
                 literalExpr.borrow()->GetAsIntUnsafe());
}

void test5() {
  VkTestSectionStart("Parsing using decl");

  string file = R"995plz(using ty = int32;)995plz";
  vector<string> lines = { file };
  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);
  std::vector<Frontend::Token> tokens = lexer.GetAndReset();
  testContext.SetParsingTokenStream(tokens);
  sona::owner<Syntax::Decl> decl = testContext.ParseUsingDecl();
  VkAssertEquals(Syntax::Node::NodeKind::CNK_UsingDecl,
                   decl.borrow()->GetNodeKind());

  VkAssertFalse(diag.HasPendingDiags());
  VkAssertNotEquals(nullptr, decl.borrow());

  sona::owner<Syntax::UsingDecl> usingDecl =
      decl.cast_unsafe<Syntax::UsingDecl>();
  VkAssertEquals("ty", usingDecl.borrow()->GetName());
  VkAssertEquals(Syntax::Type::NodeKind::CNK_BasicType,
                 usingDecl.borrow()->GetAliasee()->GetNodeKind());

}

int main() {
  VkTestStart();

  test0();
  test1();
  test2();
  test3();
  test4();
  test5();

  VkTestFinish();
}
