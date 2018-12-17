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

std::string Parser::PrettyPrintToken(Token const& token) const {
  switch (token.GetTokenKind()) {
    #define TOKEN_KWD(name, rep) \
    case Token::TK_KW_##name: return rep;
    #define TOKEN_SYM(name, rep) \
    case Token::TK_SYM_##name: return rep;
    
    case Token::TK_ID: return "identifier " + token.GetStrValueUnsafe().get();
    case Token::TK_LIT_INT: 
      return "intergral literal " + std::to_string(token.GetIntValueUnsafe());
    case Token::TK_LIT_UINT: 
      return "unsigned literal " + std::to_string(token.GetUIntValueUnsafe());
    case Token::TK_LIT_STR: 
      return "string literal " + token.GetStrValueUnsafe().get();
    case Token::TK_EOI:
      return "end of input";
  }
  
  sona_unreachable();
}

} // namespace Frontend
} // namespace ckx
