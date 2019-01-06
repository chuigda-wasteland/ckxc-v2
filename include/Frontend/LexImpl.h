#ifndef LEXIMPL_H
#define LEXIMPL_H

#include "Frontend/Token.h"
#include "Basic/Diagnose.h"

#include <memory>
#include <string>
#include <vector>

namespace ckx {
namespace Frontend {

class LexerImpl {
public:
  LexerImpl(std::string &&sourceCode, Diag::DiagnosticEngine &diag)
    : m_SourceCode(std::move(sourceCode)), m_Diag(diag) {
    LexAllTokens();
  }

  std::vector<Token> GetAndReset() noexcept;

private:
  void LexAllTokens();

  void LexIdentifier();
  void LexIdOrKeyword();
  void LexNumber();
  void LexBinNumber();
  void LexHexNumber();
  void LexString();
  void LexSymbol();
  void SkipWhitespace();

  std::uint64_t ScanInt();

  char CurChar() const noexcept;
  void NextChar() noexcept;
  char PeekOneChar() const noexcept;

  std::uint16_t GetLine() const noexcept;
  std::uint16_t GetCol() const noexcept;

  std::string m_SourceCode;
  Diag::DiagnosticEngine &m_Diag;

  std::uint16_t m_Index = 0;
  std::uint16_t m_Line = 1, m_Col = 1;
  std::vector<Token> m_TokenStream;
};

} // namespace Frontend
} // namespace ckx


#endif // LEXIMPL_H
