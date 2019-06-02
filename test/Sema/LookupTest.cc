#include "VKTestCXX.h"
#include "Frontend/Lex.h"
#include "Frontend/Parser.h"
#include "Sema/SemaPhase0.h"

#include <iostream>
#include <string>

using namespace sona;
using namespace ckx;
using namespace std;

class SemaPhase0Test : public Sema::SemaPhase0 {
public:
  SemaPhase0Test(AST::ASTContext &astContext,
                 std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
                 Diag::DiagnosticEngine &diag)
    : SemaPhase0(astContext, declContexts, diag) {}

  using SemaPhase0::LookupType;
  using SemaPhase0::GetGlobalScope;
};

void test0() {
  VkTestSectionStart("Nested name lookup");

  string f0 = R"aacaac(class A { class C { def a : int8; } })aacaac";
  string f1 = R"aacaac(class B { class C { def b : int16; } })aacaac";

  string file = f0 + "\n" + f1 + "\n";

  vector<string> lines = { f0, f1 };

  Diag::DiagnosticEngine diag("a.c", lines);
  Frontend::Lexer lexer(move(file), diag);
  std::vector<Frontend::Token> tokens = lexer.GetAndReset();

  Frontend::Parser parser(diag);
  sona::owner<Syntax::TransUnit> cst = parser.ParseTransUnit(tokens);

  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;

  SemaPhase0Test sema0(astContext, declContexts, diag);

  sona::owner<AST::TransUnitDecl> transUnit =
      sema0.ActOnTransUnit(cst.borrow());

  VkAssertFalse(diag.HasPendingDiags());
  diag.EmitDiags();

  sona::ref_ptr<AST::Type const> ACType =
      sema0.LookupType(sema0.GetGlobalScope(),
                       Syntax::Identifier(std::vector<sona::string_ref>{"A"},
                                          "C", std::vector<SourceRange>{},
                                          SourceRange(0, 0, 0)), false);

  sona::ref_ptr<AST::Type const> BCType =
      sema0.LookupType(sema0.GetGlobalScope(),
                       Syntax::Identifier(std::vector<sona::string_ref>{"B"},
                                          "C", std::vector<SourceRange>{},
                                          SourceRange(0, 0, 0)), false);

  VkAssertNotEquals(nullptr, ACType);
  VkAssertNotEquals(nullptr, BCType);
  VkAssertEquals(AST::Type::TypeId::TI_UserDefined, ACType->GetTypeId());
  VkAssertEquals(AST::Type::TypeId::TI_UserDefined, BCType->GetTypeId());

  sona::ref_ptr<AST::TypeDecl const>
      ACDecl = ACType.cast_unsafe<AST::UserDefinedType const>()->GetTypeDecl();
  sona::ref_ptr<AST::TypeDecl const>
      BCDecl = BCType.cast_unsafe<AST::UserDefinedType const>()->GetTypeDecl();

  VkAssertEquals("C", ACDecl->GetName());
  VkAssertEquals("C", BCDecl->GetName());

  VkAssertEquals(1uL, ACDecl->CastAsDeclContext()->GetDecls().size());
  VkAssertEquals(1uL, BCDecl->CastAsDeclContext()->GetDecls().size());

  sona::ref_ptr<AST::VarDecl const>
      ACVarADecl = (*(ACDecl->CastAsDeclContext()->GetDecls().begin()))
                   .cast_unsafe<AST::VarDecl const>();
  sona::ref_ptr<AST::VarDecl const>
      BCVarADecl = (*(BCDecl->CastAsDeclContext()->GetDecls().begin()))
                   .cast_unsafe<AST::VarDecl const>();

  VkAssertEquals(AST::Decl::DK_Var, ACVarADecl->GetDeclKind());
  VkAssertEquals(AST::Decl::DK_Var, BCVarADecl->GetDeclKind());
  VkAssertEquals(AST::BuiltinType::BuiltinTypeId::BTI_Int8,
                 ACVarADecl->GetType().cast_unsafe<AST::BuiltinType const>()
                           ->GetBuiltinTypeId());
  VkAssertEquals(AST::BuiltinType::BuiltinTypeId::BTI_Int16,
                 BCVarADecl->GetType().cast_unsafe<AST::BuiltinType const>()
                           ->GetBuiltinTypeId());
}

int main() {
  VkTestStart();

  test0();

  VkTestFinish();
}
