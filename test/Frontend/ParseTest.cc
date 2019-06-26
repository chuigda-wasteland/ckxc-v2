#include "VKTestCXX.h"
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
  using ParserImpl::ExpectAndConsume;
};

void test0() {
  VkTestSectionStart("Parsing variable declaration");

  string file = R"aacaac(def a : int16; def b : int16 const * const;)aacaac";
  vector<string> lines = { file };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  ParserTest testContext(diag);

  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  testContext.SetParsingTokenStream(tokens);
  owner<Syntax::Decl> decl = testContext.ParseVarDecl();
  testContext.ExpectAndConsume(Frontend::Token::TK_SYM_SEMI);
  owner<Syntax::Decl> decl2 = testContext.ParseVarDecl();
  testContext.ExpectAndConsume(Frontend::Token::TK_SYM_SEMI);

  VkAssertFalse(diag.HasPendingDiags());
  VkAssertNotEquals(nullptr, decl.borrow());
  VkAssertNotEquals(nullptr, decl2.borrow());
  VkAssertEquals(Syntax::Node::NodeKind::CNK_VarDecl,
                   decl.borrow()->GetNodeKind());
  VkAssertEquals(Syntax::Node::NodeKind::CNK_VarDecl,
                   decl2.borrow()->GetNodeKind());

  ref_ptr<Syntax::VarDecl> varDecl =
      decl.borrow().cast_unsafe<Syntax::VarDecl>();
  ref_ptr<Syntax::VarDecl> varDecl2 =
      decl2.borrow().cast_unsafe<Syntax::VarDecl>();
  VkAssertEquals("a", varDecl->GetName());
  VkAssertEquals("b", varDecl2->GetName());
  VkAssertEquals(Syntax::Node::NodeKind::CNK_ComposedType,
                 varDecl2->GetType()->GetNodeKind());
  ref_ptr<Syntax::ComposedType const> ty2 =
      varDecl2->GetType().cast_unsafe<Syntax::ComposedType const>();
  VkAssertEquals(3uL, ty2->GetTypeSpecifiers().size());
  VkAssertEquals(Syntax::ComposedType::TypeSpecifier::CTS_Const,
                 ty2->GetTypeSpecifiers()[0]);
  VkAssertEquals(Syntax::ComposedType::TypeSpecifier::CTS_Pointer,
                 ty2->GetTypeSpecifiers()[1]);
  VkAssertEquals(Syntax::ComposedType::TypeSpecifier::CTS_Const,
                 ty2->GetTypeSpecifiers()[2]);
  VkAssertEquals(Syntax::Node::NodeKind::CNK_BuiltinType,
                 ty2->GetRootType()->GetNodeKind());
  ref_ptr<Syntax::BuiltinType const> ty3 =
      ty2->GetRootType().cast_unsafe<Syntax::BuiltinType const>();
  VkAssertEquals(Syntax::BuiltinType::TypeKind::TK_Int16, ty3->GetTypeKind());

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
  VkAssertEquals("A", classDecl.borrow()->GetName());
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

  VkAssertEquals(Syntax::Node::NodeKind::CNK_IntLiteralExpr,
                   e.borrow()->GetNodeKind());
  VkAssertFalse(diag.HasPendingDiags());

  diag.EmitDiags();

  sona::owner<Syntax::IntLiteralExpr> literalExpr =
      e.cast_unsafe<Syntax::IntLiteralExpr>();

  VkAssertEquals(123123, literalExpr.borrow()->GetValue());
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
  VkAssertEquals(Syntax::Type::NodeKind::CNK_BuiltinType,
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
