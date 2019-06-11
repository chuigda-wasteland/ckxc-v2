#ifndef PARSER_IMPL_H
#define PARSER_IMPL_H

#include "Frontend/Lex.h"
#include "Syntax/Concrete.h"

namespace ckx {
namespace Frontend {

class ParserImpl {
public:
  ParserImpl(Diag::DiagnosticEngine &diag) : m_Diag(diag) {}

  sona::owner<Syntax::TransUnit>
  ParseTransUnit(sona::ref_ptr<std::vector<Token> const> tokenStream);

  sona::owner<Syntax::Stmt>
  ParseLine(sona::ref_ptr<std::vector<Token> const> tokenStream);

protected:
  /// @note Opening access to subclasses for test
  sona::owner<Syntax::Decl> ParseDeclOrFndef();
  sona::owner<Syntax::Decl> ParseVarDecl();
  sona::owner<Syntax::Decl> ParseClassDecl();
  sona::owner<Syntax::Decl> ParseEnumDecl();
  sona::owner<Syntax::Decl> ParseADTDecl();
  sona::owner<Syntax::Decl> ParseFuncDecl();
  sona::owner<Syntax::Decl> ParseUsingDecl();
  sona::owner<Syntax::Type> ParseType();

  sona::owner<Syntax::Expr> ParseExpr();
  sona::owner<Syntax::Expr> ParseLiteralExpr();
  sona::owner<Syntax::Expr> ParseIdRefExpr();

  sona::owner<Syntax::Expr> ParseUnaryExpr();
  sona::owner<Syntax::Expr> ParseUnaryAlgebraicExpr();
  sona::owner<Syntax::Expr> ParseSizeofExpr();
  sona::owner<Syntax::Expr> ParseAlignofExpr();
  sona::owner<Syntax::Expr> ParseCastExpr();
  sona::owner<Syntax::Expr> ParsePostfixExpr();

  sona::owner<Syntax::Expr>
  ParseFuncCallExpr(sona::owner<Syntax::Expr> &&parsedCallee);
  sona::owner<Syntax::Expr>
  ParseArraySubscriptExpr(sona::owner<Syntax::Expr> &&arr);
  sona::owner<Syntax::Expr>
  ParseMemberAccessExpr(sona::owner<Syntax::Expr> &&base);

  sona::owner<Syntax::Expr> ParseBinaryExpr(std::uint16_t prevPrec);

  void ParseEnumerator(std::vector<Syntax::EnumDecl::Enumerator> &enumerators);
  void ParseDataConstructor(
      std::vector<Syntax::ADTDecl::ValueConstructor>& dataConstructors);

  sona::owner<Syntax::Type> ParseBuiltinType();
  sona::owner<Syntax::Type> ParseUserDefinedType();
  Syntax::Identifier ParseIdentifier();

  void
  SetParsingTokenStream(sona::ref_ptr<std::vector<Token> const> tokenStream);

  Token const& CurrentToken() const noexcept;
  Token const& PeekToken(size_t peekCount = 1) const noexcept;
  void ConsumeToken() noexcept;

  bool Expect(Token::TokenKind tokenKind) const noexcept;
  bool ExpectAndConsume(Token::TokenKind tokenKind) noexcept;

private:
  sona::optional<std::pair<sona::strhdl_t, SourceRange>> ExpectTagId();

  void SkipTo(Token::TokenKind tokenKind);
  void SkipToAnyOf(std::initializer_list<Token::TokenKind> const& tokenKinds);

  template <typename Cond> void SkipUntil(Cond cond);

  sona::strhdl_t PrettyPrintToken(Token const &token) const;

  static Syntax::BuiltinType::TypeKind
  EvaluateIntTypeKind(std::int64_t i) noexcept;
  static Syntax::BuiltinType::TypeKind
  EvaluateUIntTypeKind(std::uint64_t u) noexcept;
  static Syntax::BuiltinType::TypeKind
  EvaluateFloatTypeKind(double d) noexcept;

  Diag::DiagnosticEngine &m_Diag;
  sona::ref_ptr<std::vector<Token> const> m_ParsingTokenStream = nullptr;
  size_t m_Index;
};

Syntax::UnaryOperator TokenToUnary(Frontend::Token::TokenKind token) noexcept;
Syntax::BinaryOperator TokenToBinary(Frontend::Token::TokenKind token) noexcept;
Syntax::CastOperator TokenToCastOp(Frontend::Token::TokenKind token) noexcept;

} // namespace Frontend
} // namespace ckx

#endif // PARSER_IMPL_H
