#include "Frontend/Lex.h"

#include <sstream>
#include <cctype>

namespace ckx {
namespace Frontend {

std::vector<Token> Lexer::GetAndReset() noexcept {
  return std::move(m_TokenStream);
}

void Lexer::LexAllTokens() {
  while (CurChar() != '\0') {
    switch (CurChar()) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V':
    case 'W': case 'X': case 'Y': case 'Z':
      LexIdentifier();
      break;
    }
  }
}

void Lexer::LexIdentifier() {
  std::stringstream stream;
  std::uint64_t col1 = GetCol();
  while (std::isdigit(CurChar()) || std::isalpha(CurChar())
         || CurChar() == '_' || CurChar() == '$') {
    stream.put(CurChar());
    NextChar();
  }
  std::uint64_t col2 = GetCol();

  std::string ret;
  stream >> ret;

  m_TokenStream.emplace_back(Token::TK_ID,
                             SourceRange(GetLine(), col1, col2), ret);
}

char Lexer::CurChar() const noexcept {
  return m_SourceCode[m_Index];
}

void Lexer::NextChar() noexcept {
  sona_assert(m_SourceCode[m_Index] != '\0');
  if (CurChar() == '\n') {
    ++m_Line;
    m_Col = 1;
  }
  else if (CurChar() == '\t') {
    m_Col += 8;
  }
  else {
    ++m_Col;
  }
  ++m_Index;
}

char Lexer::PeekOneChar() const noexcept {
  sona_assert(m_SourceCode[m_Index] != '\0');
  return m_SourceCode[m_Index + 1];
}

uint64_t Lexer::GetLine() const noexcept {
  return m_Line;
}

uint64_t Lexer::GetCol() const noexcept {
  return m_Col;
}

} // namespace Frontend
} // namespace ckx
