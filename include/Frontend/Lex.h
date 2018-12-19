#ifndef LEX_H
#define LEX_H

#include "Frontend/Token.h"
#include "Basic/Diagnose.h"
#include "sona/pointer_plus.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ckx {
namespace Frontend {

class LexerImpl;

class Lexer {
public:
  Lexer(std::string &&sourceCode, Diag::DiagnosticEngine &diag);
  std::vector<Token> GetAndReset() noexcept;

  ~Lexer();

private:
  sona::owner<LexerImpl> m_LexerImpl;
};

} // namespace Frontend
} // namespace ckx

#endif // LEX_H
