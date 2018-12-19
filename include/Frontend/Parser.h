#ifndef PARSER_H
#define PARSER_H

#include "Frontend/Lex.h"
#include "Syntax/CST.h"

namespace ckx {
namespace Frontend {

class ParserImpl;

class Parser {
public:
  Parser(Diag::DiagnosticEngine &diag);

  sona::owner<Syntax::TransUnit>
  ParseTransUnit(sona::ref_ptr<std::vector<Token> const> tokenStream);

  sona::owner<Syntax::Stmt>
  ParseLine(sona::ref_ptr<std::vector<Token> const> tokenStream);

  ~Parser();

private:
  sona::owner<ParserImpl> m_ParserImpl;
};

} // namespace Frontend
} // namespace ckx

#endif // PARSER_H
