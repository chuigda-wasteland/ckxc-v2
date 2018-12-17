#include "Frontend/Parser.h"

using namespace sona;

namespace ckx {
namespace Frontend {

owner<Syntax::TransUnit>
Parser::ParseTransUnit(ref_ptr<std::vector<Token> const> tokenStream) {
  m_ParsingTokenStream = tokenStream;
  return nullptr;
}

Token const& Parser::CurrentToken() const noexcept {
  return m_ParsingTokenStream.get()[m_Index];
}

Token const& Parser::PeekToken(size_t peekCount) const noexcept {
  return m_ParsingTokenStream.get()[peekCount];
}

void Parser::ConsumeToken() noexcept {
  m_Index++;
}

bool Parser::Expect(Token::TokenKind tokenKind) const noexcept {
  if (CurrentToken().GetTokenKind() == tokenKind) {
    return true;
  }

  return false;
}

bool Parser::ExpectAndConsume(Token::TokenKind tokenKind) noexcept {
  return false;
}

} // namespace Frontend
} // namespace ckx
