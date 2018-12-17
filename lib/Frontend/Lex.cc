#include "Frontend/Lex.h"

#include <sstream>
#include <cctype>
#include <cmath>

namespace ckx {
namespace Frontend {

std::vector<Token> Lexer::GetAndReset() noexcept {
  return std::move(m_TokenStream);
}

void Lexer::LexAllTokens() {
  while (CurChar() != '\0') {
    switch (CurChar()) {
    case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
    case 'i': case 'u':
      LexIdOrKeyword();
      break;

    case 'a': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't': case 'v': case 'w':
    case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V':
    case 'W': case 'X': case 'Y': case 'Z':
      LexIdentifier();
      break;

    case '0':
      if (PeekOneChar() == 'x' || PeekOneChar() == 'X') {
        LexHexNumber();
      }
      else if (PeekOneChar() == 'b'|| PeekOneChar() == 'B') {
        LexBinNumber();
      }
      else {
        LexNumber();
      }
      break;

    case '1': case '2': case '3': case '4': case '5': case '6':
    case '7': case '8': case '9':
      LexNumber();
      break;

    case '"':
      LexString();
      break;

    case '{': case '}': case '(': case ')': case ',': case ';': case ':':
      LexSymbol();
      break;

    case ' ': case '\t': case '\v': case '\f': case '\n':
      SkipWhitespace();
      break;
    }
  }
}

void Lexer::LexIdOrKeyword() {
  static std::unordered_map<std::string, Token::TokenKind> KeywordMaps
  { 
    #define TOKEN_KWD(name, rep) {rep, Token::TK_KW_##name},
    #include "Frontend/Tokens.def"
    {"$(40490fd0)$", Token::TK_INVALID}
  };

  std::stringstream stream;
  std::uint64_t col1 = GetCol();
  while (std::isdigit(CurChar()) || std::isalpha(CurChar())
         || CurChar() == '_' || CurChar() == '$') {
    stream.put(CurChar());
    NextChar();
  }
  std::uint64_t col2 = GetCol();

  std::string str;
  stream >> str;

  auto it = KeywordMaps.find(str);
  if (it != KeywordMaps.cend()) {
    m_TokenStream.emplace_back(it->second,
                               SourceRange(GetLine(), col1, col2), str);
    return;
  }

  m_TokenStream.emplace_back(Token::TK_ID,
                             SourceRange(GetLine(), col1, col2), str);
}

void Lexer::LexNumber() {
  std::uint64_t col1 = GetCol();
  int64_t integralPart = ScanInt();

  if (CurChar() != '.' && CurChar() != 'E' && CurChar() != 'e') {
    m_TokenStream.emplace_back(Token::TK_LIT_INT,
                               SourceRange(GetLine(), col1, GetCol()),
                               integralPart);
    return;
  }

  double floatingPart = integralPart;
  if (CurChar() == '.') {
    if (!isdigit(PeekOneChar())) {
      m_Diag.Diag(Diag::DiagnosticEngine::DIR_Error,
                  Diag::Format(Diag::DMT_ErrExpectedDigit, {}),
                  SourceRange(GetLine(), GetCol(), GetCol() + 1));
      m_TokenStream.emplace_back(Token::TK_LIT_FLOAT,
                                 SourceRange(GetLine(), col1, GetCol()),
                                 floatingPart);
      return;
    }

    NextChar();
    double r = ScanInt();
    while (r > 1) {
      r = r / 10;
    }
    floatingPart += r;
  }

  if (CurChar() == 'e' || CurChar() == 'E') {
    if (!isdigit(PeekOneChar())) {
      m_Diag.Diag(Diag::DiagnosticEngine::DIR_Error,
                  Diag::Format(Diag::DMT_ErrExpectedDigit, {}),
                  SourceRange(GetLine(), GetCol(), GetCol() + 1));
      m_TokenStream.emplace_back(Token::TK_LIT_FLOAT,
                                 SourceRange(GetLine(), col1, GetCol()),
                                 floatingPart);
      return;
    }

    NextChar();
    int exp = ScanInt();
    floatingPart *= std::pow(10, exp);
  }

  m_TokenStream.emplace_back(Token::TK_LIT_FLOAT,
                             SourceRange(GetLine(), col1, GetCol()),
                             floatingPart);
}

void Lexer::LexBinNumber() {
  sona_assert(CurChar() == '0' &&
              (PeekOneChar() == 'x' || PeekOneChar() == 'X'));
  std::uint64_t col1 = GetCol();
  NextChar();
  NextChar();

  int64_t value = 0;
  while (CurChar() == '0' || CurChar() == '1') {
    value *= 2;
    value += CurChar() - '0';
  }

  if (isalnum(CurChar()) || CurChar() == '_') {
    m_Diag.Diag(Diag::DiagnosticEngine::DIR_Error,
                Diag::Format(
                  Diag::DMT_ErrUnexpectedCharInContext,
                  { std::to_string(CurChar()), "bin number" }),
                SourceRange(GetLine(), GetCol(), GetCol()+1));
    /// @todo add proper skipping
  }

  m_TokenStream.emplace_back(Token::TK_LIT_INT,
                             SourceRange(GetLine(), col1, GetCol()), value);
}

void Lexer::LexHexNumber() {
  sona_assert(CurChar() == '0' &&
              (PeekOneChar() == 'b' || PeekOneChar() == 'B'));
  std::uint64_t col1 = GetCol();
  NextChar();
  NextChar();

  int64_t value = 0;
  while (isxdigit(CurChar())) {
    value *= 16;
    if (isalpha(CurChar())) {
      value += (std::toupper(CurChar()) - 'A') + 10;
    }
    else {
      value += CurChar() - '0';
    }
  }

  if (isalpha(CurChar()) || CurChar() == '_') {
    m_Diag.Diag(Diag::DiagnosticEngine::DIR_Error,
                Diag::Format(
                  Diag::DMT_ErrUnexpectedCharInContext,
                  { std::to_string(CurChar()), "hex number" }),
                SourceRange(GetLine(), GetCol(), GetCol()+1));
    /// @todo add proper skipping
  }

  m_TokenStream.emplace_back(Token::TK_LIT_INT,
                             SourceRange(GetLine(), col1, GetCol()), value);
}

void Lexer::LexString() {
  sona_assert(CurChar() == '"');
  std::uint64_t col1 = GetCol();
  NextChar();

  std::string str;
  while (CurChar() != '\0' && CurChar() != '"') {
    if (CurChar() == '\\') {
      switch (PeekOneChar()) {
      case 'a': str.push_back('\a'); NextChar(); NextChar(); break;
      case 'b': str.push_back('\b'); NextChar(); NextChar(); break;
      case 'n': str.push_back('\n'); NextChar(); NextChar(); break;
      case 'r': str.push_back('\r'); NextChar(); NextChar(); break;
      case 'v': str.push_back('\v'); NextChar(); NextChar(); break;
      case 't': str.push_back('\t'); NextChar(); NextChar(); break;
      case 'f': str.push_back('\f'); NextChar(); NextChar(); break;
      case '"': str.push_back('"');  NextChar(); NextChar(); break;
      case '0': str.push_back('\0'); NextChar(); NextChar(); break;
      case '\\': str.push_back('\\'); NextChar(); NextChar(); break;
      default:
        m_Diag.Diag(Diag::DiagnosticEngine::DIR_Warning0,
                    Diag::Format(Diag::DMT_WarnInvalidConversion,
                                 { std::to_string(PeekOneChar()) }),
                    SourceRange(GetLine(), GetCol()+1, GetCol()+2));
        str.push_back(CurChar());
        str.push_back(PeekOneChar());
        NextChar(); NextChar();
      }
      continue;
    }

    str.push_back(CurChar());
    NextChar();
  }

  if (CurChar() == '\0') {
    m_Diag.Diag(Diag::DiagnosticEngine::DIR_Error,
                Diag::Format(Diag::DMT_ErrUnexpectedChar, {"EOF", "string"}),
                SourceRange(GetLine(), GetCol(), GetCol()+1));
  }
  else /* if (CurChar() == '"') */ {
    NextChar();
  }

  m_TokenStream.emplace_back(Token::TK_LIT_STR,
                             SourceRange(GetLine(), col1, GetCol()), str);
}

void Lexer::LexSymbol() {
  switch (CurChar()) {
  case '{':
    m_TokenStream.emplace_back(Token::TK_SYM_LBRACE,
                               SourceRange(GetLine(), GetCol(), GetCol()+1));
    break;

  case '}':
    m_TokenStream.emplace_back(Token::TK_SYM_RBRACE,
                               SourceRange(GetLine(), GetCol(), GetCol()+1));
    break;

  case '(':
    m_TokenStream.emplace_back(Token::TK_SYM_LPAREN,
                               SourceRange(GetLine(), GetCol(), GetCol()+1));
    break;

  case ')':
    m_TokenStream.emplace_back(Token::TK_SYM_RPAREN,
                               SourceRange(GetLine(), GetCol(), GetCol()+1));
    break;

  case ',':
    m_TokenStream.emplace_back(Token::TK_SYM_COMMA,
                               SourceRange(GetLine(), GetCol(), GetCol()+1));
    break;

  case ';':
    m_TokenStream.emplace_back(Token::TK_SYM_SEMI,
                               SourceRange(GetLine(), GetCol(), GetCol()+1));
    break;

  case ':':
    m_TokenStream.emplace_back(Token::TK_SYM_COLON,
                               SourceRange(GetLine(), GetCol(), GetCol()+1));
  }
  NextChar();
}

void Lexer::SkipWhitespace() {
  while (isblank(CurChar())) {
    NextChar();
  }
}

std::uint64_t Lexer::ScanInt() {
  std::uint64_t ret = 0;
  while (std::isdigit(CurChar())) {
    ret *= 10;
    ret += CurChar() - '0';
    NextChar();
  }

  return ret;
}

void Lexer::LexIdentifier() {
  std::stringstream stream;
  std::uint64_t col1 = GetCol();

  while (std::isdigit(CurChar()) || std::isalpha(CurChar())
         || CurChar() == '_' || CurChar() == '$') {
    stream.put(CurChar());
    NextChar();
  }

  if (CurChar() == '!') {
    stream.put(CurChar());
    NextChar();
  }

  std::string str;
  stream >> str;

  m_TokenStream.emplace_back(Token::TK_ID,
                             SourceRange(GetLine(), col1, GetCol()), str);
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
