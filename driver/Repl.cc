#include "Frontend/Lex.h"
#include "Frontend/Parser.h"
#include "Sema/SemaPhase0.h"
#include "Sema/SemaPhase1.h"
#include "Backend/ReplInterpreter.h"

#include "sona/strutil.h"
#include <fstream>
#include <iostream>

using namespace sona;
using namespace ckx;
using namespace std;

class SemaPhase0ForRepl : public Sema::SemaPhase0 {
public:
  SemaPhase0ForRepl(AST::ASTContext &astContext,
                    std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
                    Diag::DiagnosticEngine &diag)
    : SemaPhase0(astContext, declContexts, diag) {}
  using SemaPhase0::ActOnVarDecl;
};

class SemaPhase1ForRepl : public Sema::SemaPhase1 {
public:
  SemaPhase1ForRepl(AST::ASTContext &astContext,
                    std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
                    Diag::DiagnosticEngine &diag)
    : SemaPhase1(astContext, declContexts, diag) {}
  using SemaPhase1::ActOnExpr;
  using SemaPhase1::GetCurrentScope;
};

int main() {
  AST::ASTContext astContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> declContexts;

  Backend::ReplInterpreter replInterp;

  for(;;) {
    vector<string> lines;
    string line;

    cerr << "[ckxc-v2-repl *Basic] λ ";
    getline(cin, line);
    if (line == "" || line == "quit" || line == "exit") {
      cerr << endl << "  Moriturus te saluto." << endl;
      return 0;
    }

    lines.push_back(line);

    Diag::DiagnosticEngine diag("<repl-input>", lines);
    Frontend::Lexer lexer(std::move(line), diag);
    if (diag.HasPendingDiags()) {
      if (diag.HasPendingError()) {
        diag.EmitDiags();
        return -1;
      }
      diag.EmitDiags();
    }

    vector<Frontend::Token> tokens = lexer.GetAndReset();
    Frontend::Parser parser(diag);
    SemaPhase0ForRepl sp0(astContext, declContexts, diag);
    SemaPhase1ForRepl sp1(astContext, declContexts, diag);

    if (tokens.front().GetTokenKind() == Frontend::Token::TK_KW_def) {
      owner<Syntax::VarDecl> decl = parser.ParseVarDecl(tokens);
      // owner<AST::VarDecl> decl1 =
      //    sp0.ActOnVarDecl(decl.borrow()).first.cast_unsafe<AST::VarDecl>();
      // replInterp.DefineVar(decl1.borrow());
      if (diag.HasPendingDiags()) {
        diag.EmitDiags();
        continue;
      }
      cerr << "  Sorry, variable decalrations are not supported yet." << endl;
    }
    else {
      owner<Syntax::Expr> expr = parser.ParseExpr(tokens);
      if (diag.HasPendingDiags()) {
        diag.EmitDiags();
        continue;
      }
      owner<AST::Expr> expr1 = sp1.ActOnExpr(sp1.GetCurrentScope(),
                                             expr.borrow());
      if (diag.HasPendingDiags()) {
        diag.EmitDiags();
        continue;
      }
      Backend::ReplValue value = expr1.borrow()->Accept(replInterp).borrow()
                                               ->GetValue<Backend::ReplValue>();
      cerr << "  Result: " << value.GetIntValue() << endl;
    }
  }
}
