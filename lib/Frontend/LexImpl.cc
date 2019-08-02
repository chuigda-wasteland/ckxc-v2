#include "Frontend/Lex.h"
#include "Frontend/LexImpl.h"

#include <sstream>
#include <cctype>
#include <cmath>

namespace ckx {
namespace Frontend {

void LexerImpl::LexAllTokens() {
  while (CurChar() != '\0') {
    switch (CurChar()) {
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'h': case 'i': case 'n': case 'q': case 'r': case 's':
    case 't': case 'u':  case 'v':
      LexIdOrKeyword();
      break;

    case 'g': case 'j': case 'k': case 'l': case 'm': case 'o': case 'p':
    case 'w': case 'x': case 'y': case 'z':
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

    case '\'':
      LexChar();
      break;

    case '"':
      LexString();
      break;

    case '{': case '}': case '(': case ')': case ',': case ';': case ':':
    case '[': case ']': case '=': case '.': case '+': case '-': case '*':
    case '&': case '<': case '>': case '!': case '~': case '^': case '/':
    case '%': case '|':
      LexSymbol();
      break;

    case ' ': case '\t': case '\v': case '\f': case '\n':
      SkipWhitespace();
      break;

    default:
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrUnexpectedChar,
                               { std::to_string(CurChar()) }),
                  CurCharRange());
      NextChar();
    }
  }

  m_TokenStream.emplace_back(Token::TK_EOI, CurCharRange());
}

void LexerImpl::LexIdOrKeyword() {
  static std::unordered_map<std::string, Token::TokenKind> KeywordMaps
  {
    #define TOKEN_KWD(name, rep) {rep, Token::TK_KW_##name},
    #include "Frontend/Tokens.def"
    {"$(40490fd0)$", Token::TK_INVALID}
  };

  std::uint16_t col1 = GetCol();
  std::string str = ScanIdString();

  auto it = KeywordMaps.find(str);
  if (it != KeywordMaps.cend()) {
    m_TokenStream.emplace_back(it->second,
                               SourceRange(GetLine(), col1, GetCol()), str);
    return;
  }

  m_TokenStream.emplace_back(Token::TK_ID,
                             SourceRange(GetLine(), col1, GetCol()), str);
}

void LexerImpl::LexNumber() {
  std::uint16_t col1 = GetCol();
  int64_t integralPart = ScanInt();

  if (CurChar() != '.' && CurChar() != 'E' && CurChar() != 'e') {
    m_TokenStream.emplace_back(Token::TK_LIT_INT,
                               SourceRange(GetLine(), col1, GetCol()),
                               integralPart);
    return;
  }

  double floatingPart = static_cast<double>(integralPart);
  if (CurChar() == '.') {
    if (!isdigit(PeekOneChar())) {
      m_Diag.Diag(Diag::DIR_Error, Diag::Format(Diag::DMT_ErrExpectedDigit, {}),
                  SourceRange(GetLine(), GetCol(), GetCol() + 1));
      m_TokenStream.emplace_back(Token::TK_LIT_FLOAT,
                                 SourceRange(GetLine(), col1, GetCol()),
                                 floatingPart);
      return;
    }

    NextChar();
    double r = static_cast<double>(ScanInt());
    while (r > 1) {
      r = r / 10;
    }
    floatingPart += r;
  }

  if (CurChar() == 'e' || CurChar() == 'E') {
    if (!isdigit(PeekOneChar())) {
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrExpectedDigit, {}),
                  SourceRange(GetLine(), GetCol(), GetCol() + 1));
      m_TokenStream.emplace_back(Token::TK_LIT_FLOAT,
                                 SourceRange(GetLine(), col1, GetCol()),
                                 floatingPart);
      return;
    }

    NextChar();
    uint64_t exp = ScanInt();
    floatingPart *= std::pow(10, exp);
  }

  m_TokenStream.emplace_back(Token::TK_LIT_FLOAT,
                             SourceRange(GetLine(), col1, GetCol()),
                             floatingPart);
}

void LexerImpl::LexBinNumber() {
  sona_assert(CurChar() == '0' &&
              (PeekOneChar() == 'x' || PeekOneChar() == 'X'));
  std::uint16_t col1 = GetCol();
  NextChar();
  NextChar();

  int64_t value = 0;
  while (CurChar() == '0' || CurChar() == '1') {
    value *= 2;
    value += CurChar() - '0';
  }

  if (isalnum(CurChar()) || CurChar() == '_') {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(
                  Diag::DMT_ErrUnexpectedCharInContext,
                  { std::to_string(CurChar()), "binary number" }),
                CurCharRange());
    while (CurChar() != '\0' && (isdigit(CurChar()) || isalpha(CurChar()))) {
      NextChar();
    }
  }

  m_TokenStream.emplace_back(Token::TK_LIT_INT,
                             SourceRange(GetLine(), col1, GetCol()), value);
}

void LexerImpl::LexHexNumber() {
  sona_assert(CurChar() == '0' &&
              (PeekOneChar() == 'b' || PeekOneChar() == 'B'));
  std::uint16_t col1 = GetCol();
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
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(
                  Diag::DMT_ErrUnexpectedCharInContext,
                  { std::to_string(CurChar()), "hex number" }),
                CurCharRange());
    while (CurChar() != '\0' && (isdigit(CurChar()) || isalpha(CurChar()))) {
      NextChar();
    }
  }

  m_TokenStream.emplace_back(Token::TK_LIT_INT,
                             SourceRange(GetLine(), col1, GetCol()), value);
}

void LexerImpl::LexChar() {
  sona_assert(CurChar() == '\'');
  std::uint16_t col1 = GetCol();
  NextChar();
  char ch = '\0';
  if (CurChar() == '\\') {
    switch (PeekOneChar()) {
    case 'a':  ch = '\a'; NextChar(); NextChar(); break;
    case 'b':  ch = '\b'; NextChar(); NextChar(); break;
    case 'n':  ch = '\n'; NextChar(); NextChar(); break;
    case 'r':  ch = '\r'; NextChar(); NextChar(); break;
    case 'v':  ch = '\v'; NextChar(); NextChar(); break;
    case 't':  ch = '\t'; NextChar(); NextChar(); break;
    case 'f':  ch = '\f'; NextChar(); NextChar(); break;
    case '\'': ch = '\'';  NextChar(); NextChar(); break;
    case '0':  ch = '\0'; NextChar(); NextChar(); break;
    case '\\': ch = '\\'; NextChar(); NextChar(); break;
    default:
      m_Diag.Diag(Diag::DIR_Warning0,
                  Diag::Format(Diag::DMT_WarnInvalidConversion,
                               { std::to_string(PeekOneChar()) }),
                  SourceRange(GetLine(), GetCol()+1, GetCol()+2));
      ch = CurChar();
      NextChar(); NextChar();
    }
  }
  else {
    ch = CurChar();
    NextChar();
  }

  if (CurChar() != '\'') {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrExpectedCharGot,
                             {std::to_string(CurChar()), "'"}),
                SourceRange(GetLine(), GetCol(), GetCol()));
  }
  else {
    NextChar();
  }

  m_TokenStream.emplace_back(Token::TK_LIT_CHAR,
                             SourceRange(GetLine(), col1, GetCol()), ch);
}

void LexerImpl::LexString() {
  sona_assert(CurChar() == '"');
  std::uint16_t col1 = GetCol();
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
        m_Diag.Diag(Diag::DIR_Warning0,
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
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrUnexpectedChar, {"EOF", "string"}),
                CurCharRange());
  }
  else /* if (CurChar() == '"') */ {
    NextChar();
  }

  m_TokenStream.emplace_back(Token::TK_LIT_STR,
                             SourceRange(GetLine(), col1, GetCol()), str);
}

void LexerImpl::LexSymbol() {
  switch (CurChar()) {
  case '{':
    m_TokenStream.emplace_back(Token::TK_SYM_LBRACE, CurCharRange()); break;

  case '}':
    m_TokenStream.emplace_back(Token::TK_SYM_RBRACE, CurCharRange()); break;

  case '[':
    m_TokenStream.emplace_back(Token::TK_SYM_LBRACKET, CurCharRange()); break;

  case ']':
    m_TokenStream.emplace_back(Token::TK_SYM_RBRACKET, CurCharRange()); break;

  case '(':
    m_TokenStream.emplace_back(Token::TK_SYM_LPAREN, CurCharRange()); break;

  case ')':
    m_TokenStream.emplace_back(Token::TK_SYM_RPAREN, CurCharRange()); break;

  case '<':
    switch (PeekOneChar()) {
    case '<':
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_LTLT, CurCharRange());
      break;

    case '=':
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_LTEQ, CurCharRange());
      break;

    default:
      m_TokenStream.emplace_back(Token::TK_SYM_LT, CurCharRange());
    }
    break;

  case '>':
    switch (PeekOneChar()) {
    case '>':
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_GTGT, CurCharRange());
      break;

    case '=':
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_GTEQ, CurCharRange());
      break;

    default:
      m_TokenStream.emplace_back(Token::TK_SYM_GT, CurCharRange());
    }
    break;

  case ',':
    m_TokenStream.emplace_back(Token::TK_SYM_COMMA, CurCharRange()); break;

  case ';':
    m_TokenStream.emplace_back(Token::TK_SYM_SEMI, CurCharRange()); break;

  case '~':
    m_TokenStream.emplace_back(Token::TK_SYM_WAVE, CurCharRange()); break;

  case '=':
    if (PeekOneChar() == '=') {
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_EQEQ, CurCharRange());
    }
    else {
      m_TokenStream.emplace_back(Token::TK_SYM_EQ, CurCharRange());
    }
    break;

  case '!':
    if (PeekOneChar() == '=') {
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_EXCEQ, CurCharRange());
    }
    else {
      m_TokenStream.emplace_back(Token::TK_SYM_EXCLAIM, CurCharRange());
    }
    break;

  case '.':
    m_TokenStream.emplace_back(Token::TK_SYM_DOT, CurCharRange()); break;

  case '+':
    if (PeekOneChar() == '+') {
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_DPLUS, CurCharRange());
    }
    else {
      m_TokenStream.emplace_back(Token::TK_SYM_PLUS, CurCharRange());
    }
    break;

  case '-':
    if (PeekOneChar() == '-') {
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_DMINUS, CurCharRange());
    }
    else {
      m_TokenStream.emplace_back(Token::TK_SYM_MINUS, CurCharRange());
    }
    break;

  case '&':
    if (PeekOneChar() == '&') {
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_DAMP, CurCharRange());
    }
    else {
      m_TokenStream.emplace_back(Token::TK_SYM_AMP, CurCharRange());
    }
    break;

  case '|':
    if (PeekOneChar() == '|') {
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_DPIPE, CurCharRange());
    }
    else {
      m_TokenStream.emplace_back(Token::TK_SYM_PIPE, CurCharRange());
    }
    break;

  case '^':
    if (PeekOneChar() == '^') {
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_DTIP, CurCharRange());
    }
    else {
      m_TokenStream.emplace_back(Token::TK_SYM_TIP, CurCharRange());
    }
    break;

  case ':':
    if (PeekOneChar() == ':') {
      NextChar();
      m_TokenStream.emplace_back(Token::TK_SYM_DCOLON, CurCharRange());
    }
    else {
      m_TokenStream.emplace_back(Token::TK_SYM_COLON, CurCharRange());
    }
    break;

  case '*':
    m_TokenStream.emplace_back(Token::TK_SYM_ASTER, CurCharRange()); break;

  case '/':
    m_TokenStream.emplace_back(Token::TK_SYM_SLASH, CurCharRange()); break;

  case '%':
    m_TokenStream.emplace_back(Token::TK_SYM_PERCENT, CurCharRange()); break;
  }

  NextChar();
}

void LexerImpl::SkipWhitespace() {
  while (isblank(CurChar()) || CurChar() == '\n') {
    NextChar();
  }
}

std::string LexerImpl::ScanIdString() {
  std::stringstream stream;
  while (std::isdigit(CurChar()) || std::isalpha(CurChar())
         || CurChar() == '_' || CurChar() == '$') {
    stream.put(CurChar());
    NextChar();
  }

  while (CurChar() == '!' || CurChar() == '?') {
    stream.put(CurChar());
    NextChar();
  }

  std::string ret;
  stream >> ret;
  return ret;
}

std::uint64_t LexerImpl::ScanInt() {
  std::uint64_t ret = 0;
  while (std::isdigit(CurChar())) {
    ret *= 10;
    ret += CurChar() - '0'; NextChar();
  }
  return ret;
}

void LexerImpl::LexIdentifier() {
  std::uint16_t col1 = GetCol();
  std::string str = ScanIdString();
  m_TokenStream.emplace_back(Token::TK_ID,
                             SourceRange(GetLine(), col1, GetCol()), str);
}

char LexerImpl::CurChar() const noexcept {
  return m_SourceCode[m_Index];
}

void LexerImpl::NextChar() noexcept {
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

char LexerImpl::PeekOneChar() const noexcept {
  sona_assert(m_SourceCode[m_Index] != '\0');
  return m_SourceCode[m_Index + 1];
}

SourceRange LexerImpl::CurCharRange() const noexcept {
  return SourceRange(GetLine(), GetCol(), GetCol()+1);
}

uint16_t LexerImpl::GetLine() const noexcept {
  return m_Line;
}

uint16_t LexerImpl::GetCol() const noexcept {
  return m_Col;
}

} // namespace Frontend
} // namespace ckx
