#include "Frontend/Lex.h"

namespace ckx {
namespace Frontend {

std::vector<Token> Lexer::GetAndReset() noexcept {
  return std::move(m_TokenStream);
}

void Lexer::LexAllTokens() {
  while (CurChar() != '\0') {
    switch (CurChar()) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'I': case 'J': case 'K': case 'L': case 'M':
    case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S':
    case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y':
    case 'Z':
      break;
    }
  }
}

char Lexer::CurChar() const noexcept {
  return m_SourceCode[m_Index];
}

void Lexer::NextChar() noexcept {
  sona_assert(m_SourceCode[m_Index] != '\0');
  ++m_Index;
}

char Lexer::PeekOneChar() const noexcept {
  sona_assert(m_SourceCode[m_Index] != '\0');
  return m_SourceCode[m_Index + 1];
}

} // namespace Frontend
} // namespace ckx
