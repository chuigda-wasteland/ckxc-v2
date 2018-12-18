#ifndef PARSER_H
#define PARSER_H

#include "Frontend/Lex.h"
#include "Syntax/CST.h"

namespace ckx {
namespace Frontend {

class Parser {
public:
  Parser(Diag::DiagnosticEngine &diag) : m_Diag(diag) {}

  sona::owner<Syntax::TransUnit>
  ParseTransUnit(sona::ref_ptr<std::vector<Token> const> tokenStream);

  sona::owner<Syntax::Stmt>
  ParseLine(sona::ref_ptr<std::vector<Token> const> tokenStream);

protected:
  /// @note Opening access to subclasses for test
  sona::owner<Syntax::Decl> ParseDeclOrFndef();
  sona::owner<Syntax::Decl> ParseVarDecl();
  sona::owner<Syntax::Type> ParseType();

  sona::owner<Syntax::Type> ParseBuiltinType();
  sona::owner<Syntax::Type> ParseUserDefinedType();

  void
  SetParsingTokenStream(sona::ref_ptr<std::vector<Token> const> tokenStream);

private:
  Diag::DiagnosticEngine &m_Diag;

  Token const& CurrentToken() const noexcept;
  Token const& PeekToken(size_t peekCount = 1) const noexcept;
  void ConsumeToken() noexcept;

  bool Expect(Token::TokenKind tokenKind) const noexcept;
  bool ExpectAndConsume(Token::TokenKind tokenKind) noexcept;

  sona::string_ref PrettyPrintToken(Token const &token) const;
  sona::string_ref PrettyPrintTokenKind(Token::TokenKind tokenKind) const;

  sona::ref_ptr<std::vector<Token> const> m_ParsingTokenStream = nullptr;
  size_t m_Index;
};

} // namespace Frontend
} // namespace ckx

#endif // PARSER_H
