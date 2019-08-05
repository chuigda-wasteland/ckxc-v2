#include "Frontend/Parser.h"
#include "Frontend/ParserImpl.h"

namespace ckx {
namespace Frontend {

Parser::Parser(Diag::DiagnosticEngine &diag)
  : m_ParserImpl(new ParserImpl(diag)) {
}

sona::owner<Syntax::TransUnit>
Parser::ParseTransUnit(sona::ref_ptr<std::vector<Token> const> tokenStream) {
  return m_ParserImpl.borrow()->ParseTransUnit(tokenStream);
}

sona::owner<Syntax::Expr>
Parser::ParseExpr(sona::ref_ptr<const std::vector<Token> > tokenStream) {
  return m_ParserImpl.borrow()->ParseReplExpr(tokenStream);
}

sona::owner<Syntax::VarDecl>
Parser::ParseVarDecl(sona::ref_ptr<const std::vector<Token> > tokenStream) {
  return m_ParserImpl.borrow()->ParseReplVarDecl(tokenStream);
}

Parser::~Parser() {}

} // namespace Frontend
} // namespace ckx
