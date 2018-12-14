#ifndef TOKEN_H
#define TOKEN_H

#include "Basic/SourceRange.hpp"
#include "sona/stringref.hpp"
#include <cstdint>

namespace ckx {
namespace Frontend {

class Token {
public:
  enum TokenKind {
    TK_KW_class, TK_KW_enum, TK_KW_def, TK_KW_func,
    TK_KW_int8, TK_KW_int16, TK_KW_int32, TK_KW_int64,
    TK_KW_uint8, TK_KW_uint16, TK_KW_uint32, TK_KW_uint64,
    TK_KW_float, TK_KW_double, TK_KW_bool,

    TK_SYM_LBRACE, TK_SYM_RBRACE, TK_SYM_LPAREN, TK_SYM_RPAREN,
    TK_SYM_SEMI, TK_SYM_COMMA,

    TK_ID,

    TK_LIT_INT, TK_LIT_UINT, TK_LIT_FLOAT, TK_LIT_STR,

    TK_EOI
  };

  Token(TokenKind tokenKind, SourceRange const& sourceRange) :
    m_TokenKind(tokenKind), m_SourceRange(sourceRange) {}

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
        sona::string_ref const& StrValue)
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

  sona::string_ref const& GetStrValueUnsafe() const noexcept {
    sona_assert(GetTokenKind() == TK_LIT_STR || GetTokenKind() == TK_ID);
    return m_StrValue;
  }

private:
  TokenKind const m_TokenKind;
  SourceRange const m_SourceRange;

  union {
    std::int64_t IntValue;
    std::uint64_t UIntValue;
    double FloatValue;
  } m_Value;

  sona::string_ref m_StrValue = "";
};

} // namespace Frontend
} // namespace ckx

#endif // TOKEN_H
