#ifndef LEX_H
#define LEX_H

#include "Frontend/Token.h"

#include <memory>
#include <string>
#include <vector>

namespace ckx {
namespace Frontend {

class Lexer {
public:
  Lexer(std::string &&sourceCode) : m_SourceCode(std::move(sourceCode)) {}
  std::vector<Token> GetAndReset() noexcept;

private:
  void LexAllTokens();

  void LexIdentifier();
  void LexIdOrKeyword();
  void LexNumber();
  void LexString();

  char CurChar() const noexcept;
  void NextChar() noexcept;
  char PeekOneChar() const noexcept;

  std::uint64_t GetLine() const noexcept;
  std::uint64_t GetCol() const noexcept;

  std::string m_SourceCode;
  std::uint64_t m_Index = 0;
  std::uint64_t m_Line = 1, m_Col = 1;
  std::vector<Token> m_TokenStream;
};

} // namespace Frontend
} // namespace ckx

#endif // LEX_H
