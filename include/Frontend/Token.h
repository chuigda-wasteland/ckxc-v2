#ifndef TOKEN_H
#define TOKEN_H

#include "Basic/SourceRange.h"
#include "sona/stringref.h"
#include "sona/optional.h"
#include <cstdint>

namespace ckx {
namespace Frontend {

class Token {
public:
  enum TokenKind {
    #define TOKEN_KWD(name, rep) TK_KW_##name,
    #define TOKEN_SYM(name, rep) TK_SYM_##name,
    #define TOKEN_MISC(name, desc) TK_##name,
    
    #include "Frontend/Tokens.def"
    TK_INVALID
  };

  Token(TokenKind tokenKind, SourceRange const& sourceRange) :
    m_TokenKind(tokenKind), m_SourceRange(sourceRange),
    m_StrValue(sona::empty_optional()) {}

  Token(TokenKind tokenKind, SourceRange const& sourceRange,
        std::int64_t IntValue)
    : Token(tokenKind, sourceRange) {
    m_Value.IntValue = IntValue;
  }

  Token(TokenKind tokenKind, SourceRange const& sourceRange,
        std::uint64_t UIntValue)
    : Token(tokenKind, sourceRange) {
    m_Value.UIntValue = UIntValue;
  }

  Token(TokenKind tokenKind, SourceRange const& sourceRange,
        double FloatValue)
    : Token(tokenKind, sourceRange) {
    m_Value.FloatValue = FloatValue;
  }

  Token(TokenKind tokenKind, SourceRange const& sourceRange,
        char CharValue)
    : Token(tokenKind, sourceRange) {
    m_Value.CharValue = CharValue;
  }

  Token(TokenKind tokenKind, SourceRange const& sourceRange,
        sona::strhdl_t const& StrValue)
    : m_TokenKind(tokenKind), m_SourceRange(sourceRange),
      m_StrValue(StrValue) {}

  TokenKind const& GetTokenKind() const noexcept {
    return m_TokenKind;
  }

  SourceRange const& GetSourceRange() const noexcept {
    return m_SourceRange;
  }

  std::int64_t GetIntValueUnsafe() const noexcept {
    sona_assert(GetTokenKind() == TK_LIT_INT);
    return m_Value.IntValue;
  }

  std::uint64_t GetUIntValueUnsafe() const noexcept {
    sona_assert(GetTokenKind() == TK_LIT_UINT);
    return m_Value.UIntValue;
  }

  double GetFloatValueUnsafe() const noexcept {
    sona_assert(GetTokenKind() == TK_LIT_FLOAT);
    return m_Value.FloatValue;
  }

  char GetCharValueUnsafe() const noexcept {
    sona_assert(GetTokenKind() == TK_LIT_CHAR);
    return m_Value.CharValue;
  }

  sona::strhdl_t const& GetStrValueUnsafe() const noexcept {
    sona_assert(GetTokenKind() == TK_LIT_STR || GetTokenKind() == TK_ID);
    sona_assert(m_StrValue.has_value());
    return m_StrValue.value();
  }

private:
  TokenKind const m_TokenKind;
  SourceRange const m_SourceRange;

  union {
    std::int64_t IntValue;
    std::uint64_t UIntValue;
    double FloatValue;
    char CharValue;
  } m_Value;

  sona::optional<sona::strhdl_t> m_StrValue;
};

sona::strhdl_t PrettyPrintTokenKind(Token::TokenKind tokenKind);

} // namespace Frontend
} // namespace ckx

#endif // TOKEN_H
