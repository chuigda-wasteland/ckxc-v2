#include "VKTestCXX.h"
#include "Frontend/Lex.h"
#include "Frontend/ParserImpl.h"
#include "Sema/SemaPhase0.h"

#include <iostream>
#include <string>

using namespace sona;
using namespace ckx;
using namespace std;

class ParserTest : public Frontend::ParserImpl {
public:
  ParserTest(Diag::DiagnosticEngine &diag) : ParserImpl(diag) {}

  using ParserImpl::SetParsingTokenStream;
  using ParserImpl::ParseType;
};

class SemaPhase0Test : public Sema::SemaPhase0 {
public:
  SemaPhase0Test(AST::ASTContext &astContext,
                 std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
                 Diag::DiagnosticEngine &diag)
    : SemaPhase0(astContext, declContexts, diag) {}

  using SemaPhase0::ResolveType;
};

void test0() {
  VkTestSectionStart("Resolving composed types");

  string file = R"cnn(int16 const volatile * restrict)cnn";
  vector<string> lines = { file };
  Diag::DiagnosticEngine diag("a.c", lines);
  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;

  Frontend::Lexer lexer(move(file), diag);
  ParserTest testParser(diag);
  SemaPhase0Test testSema(astContext, declContexts, diag);

  std::vector<Frontend::Token> tokens = lexer.GetAndReset();
  testParser.SetParsingTokenStream(tokens);
  owner<Syntax::Type> sty = testParser.ParseType();

  auto pair = testSema.ResolveType(sty.borrow());
  VkAssertTrue(pair.contains_t1());

  AST::QualType qty = pair.as_t1();
  VkAssertTrue(qty.IsRestrict());
  VkAssertFalse(qty.IsConst());
  VkAssertFalse(qty.IsVolatile());
  VkAssertEquals(AST::Type::TypeId::TI_Pointer, qty.GetUnqualTy()->GetTypeId());

  sona::ref_ptr<AST::PointerType const> ptrTy
      = qty.GetUnqualTy().cast_unsafe<AST::PointerType const>();
  AST::QualType qty2 = ptrTy->GetPointee();
  VkAssertFalse(qty2.IsRestrict());
  VkAssertTrue(qty2.IsConst());
  VkAssertTrue(qty2.IsVolatile());
  VkAssertEquals(AST::Type::TypeId::TI_Builtin,
                 qty2.GetUnqualTy()->GetTypeId());
  sona::ref_ptr<AST::BuiltinType const> bty =
      qty2.GetUnqualTy().cast_unsafe<AST::BuiltinType const>();
  VkAssertEquals(AST::BuiltinType::BuiltinTypeId::BTI_Int16,
                 bty->GetBuiltinTypeId());
}

int main() {
  VkTestStart();

  test0();

  VkTestFinish();
}
