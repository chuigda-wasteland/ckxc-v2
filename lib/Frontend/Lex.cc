#include "Frontend/Lex.h"
#include "Frontend/LexImpl.h"

namespace ckx {
namespace Frontend {

Lexer::Lexer(std::string &&sourceCode, Diag::DiagnosticEngine &diag)
  : m_LexerImpl(new LexerImpl(std::move(sourceCode), diag)) {
}

std::vector<Token> Lexer::GetAndReset() noexcept {
  return m_LexerImpl.borrow()->GetAndReset();
}

Lexer::~Lexer() {}

std::vector<Token> LexerImpl::GetAndReset() noexcept {
  return std::move(m_TokenStream);
}

} // namespace Frontend
} // namespace ckx
