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

sona::owner<Syntax::Stmt>
Parser::ParseLine(sona::ref_ptr<std::vector<Token> const> tokenStream) {
  return m_ParserImpl.borrow()->ParseLine(tokenStream);
}

Parser::~Parser() {}

} // namespace Frontend
} // namespace ckx
