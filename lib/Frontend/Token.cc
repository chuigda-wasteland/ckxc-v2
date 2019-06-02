#include "Frontend/Token.h"

namespace ckx {
namespace Frontend {

sona::string_ref PrettyPrintTokenKind(Token::TokenKind tokenKind) {
  switch (tokenKind) {
#define TOKEN_KWD(name, rep) \
  case Token::TK_KW_##name: return "'" + std::string(rep) + "'";
#define TOKEN_SYM(name, rep) \
  case Token::TK_SYM_##name: return "'" + std::string(rep) + "'";
#define TOKEN_MISC(name, desc) \
  case Token::TK_##name: return desc;
#include "Frontend/Tokens.def"
  case Token::TK_INVALID: sona_unreachable();
  }
  sona_unreachable();
  return "";
}

} // namespace Frontend
} // namespace ckx
