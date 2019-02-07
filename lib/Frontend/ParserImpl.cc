#include "Frontend/ParserImpl.h"

using namespace sona;

namespace ckx {
namespace Frontend {

owner<Syntax::TransUnit>
ParserImpl::ParseTransUnit(ref_ptr<std::vector<Token> const> tokenStream) {
  SetParsingTokenStream(tokenStream);

  owner<Syntax::TransUnit> ret = new Syntax::TransUnit;
  while (CurrentToken().GetTokenKind() != Token::TK_EOI) {
    owner<Syntax::Decl> d = ParseDeclOrFndef();
    if (d.borrow() == nullptr) {
      continue;
    }
    if (d.borrow()->GetNodeKind() == Syntax::Node::NodeKind::CNK_VarDecl) {
      ExpectAndConsume(Token::TK_SYM_SEMI);
    }
    ret.borrow()->Declare(std::move(d));
  }

  return ret;
}

owner<Syntax::Stmt>
ParserImpl::ParseLine(sona::ref_ptr<std::vector<Token> const> tokenStream) {
  SetParsingTokenStream(tokenStream);
  return nullptr;
}

owner<Syntax::Decl> ParserImpl::ParseDeclOrFndef() {
  switch (CurrentToken().GetTokenKind()) {
  case Token::TK_KW_def: return ParseVarDecl();
  case Token::TK_KW_class: return ParseClassDecl();
  case Token::TK_KW_enum:
    if (PeekToken().GetTokenKind() == Token::TK_KW_class) {
      return ParseADTDecl();
    } else {
      return ParseEnumDecl();
    }
  case Token::TK_KW_func: return ParseFuncDecl();
    sona_unreachable1("not implemented");
  default:
    sona_unreachable();
  }
  return nullptr;
}

owner<Syntax::Decl> ParserImpl::ParseVarDecl() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_def);
  SourceRange defRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!Expect(Token::TK_ID)) {
    /// add proper skipping
    return nullptr;
  }

  sona::string_ref name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!ExpectAndConsume(Token::TK_SYM_COLON)) {
    return nullptr;
  }

  owner<Syntax::Type> type = ParseType();
  return new Syntax::VarDecl(name, std::move(type), defRange, nameRange);
}

owner<Syntax::Decl> ParserImpl::ParseClassDecl() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_class);
  SourceRange classRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!Expect(Token::TK_ID)) {
    /// add proper skipping
    return nullptr;
  }

  sona::string_ref name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!ExpectAndConsume(Token::TK_SYM_LBRACE)) {
    /// @todo add proper skipping
    return nullptr;
  }

  std::vector<owner<Syntax::Decl>> decls;

  while (CurrentToken().GetTokenKind() != Token::TK_EOI
         && CurrentToken().GetTokenKind() != Token::TK_SYM_RBRACE) {
    switch (CurrentToken().GetTokenKind()) {
    case Token::TK_KW_def:
      decls.push_back(ParseVarDecl());
      ExpectAndConsume(Token::TK_SYM_SEMI);
      break;
    case Token::TK_KW_class: decls.push_back(ParseClassDecl()); break;
    case Token::TK_KW_enum: decls.push_back(ParseEnumDecl()); break;
    default:
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrExpectedGot, {
                                 PrettyPrintToken(CurrentToken()),
                                 "field declaration"
                               }),
                  CurrentToken().GetSourceRange());
      ConsumeToken();
      continue;
    }
  }

  ExpectAndConsume(Token::TK_SYM_RBRACE);
  return new Syntax::ClassDecl(name, std::move(decls), classRange, nameRange);
}

owner<Syntax::Decl> ParserImpl::ParseEnumDecl() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_enum);
  SourceRange enumRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!Expect(Token::TK_ID)) {
    /// add proper skipping
    return nullptr;
  }

  sona::string_ref name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!ExpectAndConsume(Token::TK_SYM_LBRACE)) {
    /// @todo add proper skipping
    return nullptr;
  }

  std::vector<Syntax::EnumDecl::Enumerator> enumerators;

  while (CurrentToken().GetTokenKind() != Token::TK_EOI
         && CurrentToken().GetTokenKind() != Token::TK_SYM_RBRACE) {
    ParseEnumerator(enumerators);
    ExpectAndConsume(Token::TK_SYM_SEMI);
  }

  ExpectAndConsume(Token::TK_SYM_RBRACE);
  return new Syntax::EnumDecl(name, std::move(enumerators),
                              enumRange, nameRange);
}

sona::owner<Syntax::Decl> ParserImpl::ParseADTDecl() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_enum);
  sona_assert(PeekToken().GetTokenKind() == Token::TK_KW_class);

  SourceRange enumRange = CurrentToken().GetSourceRange();
  ConsumeToken();
  SourceRange classRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!Expect(Token::TK_ID)) {
    /// add proper skipping
    return nullptr;
  }

  string_ref name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!ExpectAndConsume(Token::TK_SYM_LBRACE)) {
    return nullptr;
  }

  std::vector<Syntax::ADTDecl::DataConstructor> dataConstructors;

  while (CurrentToken().GetTokenKind() != Token::TK_EOI
         && CurrentToken().GetTokenKind() != Token::TK_SYM_RBRACE) {
    ParseDataConstructor(dataConstructors);
    ExpectAndConsume(Token::TK_SYM_SEMI);
  }

  ExpectAndConsume(Token::TK_SYM_RBRACE);

  return new Syntax::ADTDecl(name, std::move(dataConstructors),
                             enumRange, classRange, nameRange);
}

sona::owner<Syntax::Decl> ParserImpl::ParseFuncDecl() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_func);
  SourceRange funcRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!Expect(Token::TK_ID)) {
    return nullptr;
  }

  string_ref name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!ExpectAndConsume(Token::TK_SYM_LPAREN)) {
    return nullptr;
  }

  std::vector<string_ref> paramNames;
  std::vector<owner<Syntax::Type>> paramTypes;

  while (CurrentToken().GetTokenKind() != Token::TK_EOI) {
    if (!Expect(Token::TK_ID)) {
      continue;
    }
    string_ref paramName = CurrentToken().GetStrValueUnsafe();
    ConsumeToken();

    if (!ExpectAndConsume(Token::TK_SYM_COLON)) {
      continue;
    }

    owner<Syntax::Type> type = ParseType();
    if (type.borrow() == nullptr) {
      continue;
    }

    paramNames.push_back(paramName);
    paramTypes.push_back(std::move(type));

    if (CurrentToken().GetTokenKind() == Token::TK_SYM_COMMA) {
      ConsumeToken();
    }
    else {
      ExpectAndConsume(Token::TK_SYM_RPAREN);
      break;
    }
  }

  if (!ExpectAndConsume(Token::TK_SYM_COLON)) {
    return nullptr;
  }

  owner<Syntax::Type> retType = ParseType();
  ExpectAndConsume(Token::TK_SYM_SEMI);
  return new Syntax::FuncDecl(name, std::move(paramTypes),
                              std::move(paramNames), std::move(retType),
                              empty_optional(), funcRange, nameRange);
}

void ParserImpl::
ParseEnumerator(std::vector<Syntax::EnumDecl::Enumerator> &enumerators) {
  if (!Expect(Token::TK_ID)) {
    return;
  }

  sona::string_ref name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (CurrentToken().GetTokenKind() != Token::TK_SYM_EQ) {
    enumerators.emplace_back(name, nameRange);
    return;
  }

  SourceRange eqRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!Expect(Token::TK_LIT_INT)) {
    return;
  }

  int64_t value = CurrentToken().GetIntValueUnsafe();
  SourceRange valueRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  enumerators.emplace_back(name, value, nameRange, eqRange, valueRange);
}

void ParserImpl::ParseDataConstructor(
  std::vector<Syntax::ADTDecl::DataConstructor> &dataConstructors) {
  if (!Expect(Token::TK_ID)) {
    return;
  }

  sona::string_ref name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (CurrentToken().GetTokenKind() != Token::TK_SYM_LPAREN) {
    dataConstructors.emplace_back(name, nameRange);
    return;
  }

  ConsumeToken();
  sona::owner<Syntax::Type> underlyingType = ParseType();
  if (underlyingType.borrow() == nullptr) {
    return;
  }
  ExpectAndConsume(Token::TK_SYM_RPAREN);

  dataConstructors.emplace_back(name, std::move(underlyingType), nameRange);
}

owner<Syntax::Type> ParserImpl::ParseType() {
  owner<Syntax::Type> ret = nullptr;
  if (CurrentToken().GetTokenKind() == Token::TK_ID) {
    ret = ParseUserDefinedType();
  }
  else {
    ret = ParseBuiltinType();
  }

  if (ret.borrow() == nullptr) {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrExpectedGot, {
                               PrettyPrintToken(CurrentToken()),
                               "type"
                             }),
                CurrentToken().GetSourceRange());
  }

  return ret;
}

sona::owner<Syntax::Expr> ParserImpl::ParseExpr() {
  /// @todo
  return nullptr;
}

sona::owner<Syntax::Expr> ParserImpl::ParseLiteralExpr() {
  sona::owner<Syntax::Expr> ret = nullptr;
  switch (CurrentToken().GetTokenKind()) {
  case Token::TK_LIT_INT:
    ret = new Syntax::LiteralExpr(
            CurrentToken().GetIntValueUnsafe(),
            EvaluateIntTypeKind(CurrentToken().GetIntValueUnsafe()),
            CurrentToken().GetSourceRange());
    break;
  case Token::TK_LIT_UINT:
    ret = new Syntax::LiteralExpr(
            CurrentToken().GetUIntValueUnsafe(),
            EvaluateUIntTypeKind(CurrentToken().GetUIntValueUnsafe()),
            CurrentToken().GetSourceRange());
    break;
  case Token::TK_LIT_FLOAT:
    ret = new Syntax::LiteralExpr(
            CurrentToken().GetFloatValueUnsafe(),
            EvaluateFloatTypeKind(CurrentToken().GetFloatValueUnsafe()),
            CurrentToken().GetSourceRange());
    break;
  case Token::TK_LIT_STR:
    return new Syntax::StringLiteralExpr(CurrentToken().GetStrValueUnsafe(),
                                         CurrentToken().GetSourceRange());
    break;
  default:
    sona_unreachable();
  }
  ConsumeToken();
  return ret;
}

sona::owner<Syntax::Expr> ParserImpl::ParseIdRefExpr() {
  return new Syntax::IdRefExpr(ParseIdentifier());
}

sona::owner<Syntax::Expr> ParserImpl::ParseUnaryExpr() {
  switch (CurrentToken().GetTokenKind()) {
  case Token::TK_KW_sizeof:
    return ParseSizeofExpr();

  case Token::TK_KW_alignof:
    return ParseAlignofExpr();

  case Token::TK_KW_static_cast:
  case Token::TK_KW_bitcast:
  case Token::TK_KW_const_cast:
    return ParseCastExpr();

  case Token::TK_SYM_PLUS:
  case Token::TK_SYM_MINUS:
    return ParseUnaryAlgebraicExpr();

  case Token::TK_LIT_INT:
  case Token::TK_LIT_UINT:
  case Token::TK_LIT_FLOAT:
  case Token::TK_LIT_STR:
    return ParseLiteralExpr();

  case Token::TK_ID:
    return ParseIdRefExpr();

  default:
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrUnexpectedCharInContext, {
                               PrettyPrintToken(CurrentToken()),
                               "unary expression"}),
                CurrentToken().GetSourceRange());
    return nullptr;
  }
}

sona::owner<Syntax::Expr> ParserImpl::ParseUnaryAlgebraicExpr() {
  Syntax::UnaryOperator uop = TokenToUnary(CurrentToken().GetTokenKind());
  SourceRange uopRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  sona::owner<Syntax::Expr> e = ParseUnaryExpr();
  return new Syntax::UnaryAlgebraicExpr(uop, std::move(e), uopRange);
}

sona::owner<Syntax::Expr> ParserImpl::ParseSizeofExpr() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_sizeof);
  SourceRange sizeofRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!ExpectAndConsume(Token::TK_SYM_LPAREN)) {
    return nullptr;
  }

  sona::owner<Syntax::Expr> containedExpr = ParseExpr();
  ExpectAndConsume(Token::TK_SYM_RPAREN);

  return new Syntax::SizeOfExpr(std::move(containedExpr), sizeofRange);
}

sona::owner<Syntax::Expr> ParserImpl::ParseAlignofExpr() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_alignof);
  SourceRange alignofRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!ExpectAndConsume(Token::TK_SYM_LPAREN)) {
    return nullptr;
  }

  sona::owner<Syntax::Expr> containedExpr = ParseExpr();
  ExpectAndConsume(Token::TK_SYM_RPAREN);

  return new Syntax::SizeOfExpr(std::move(containedExpr), alignofRange);
}

sona::owner<Syntax::Expr> ParserImpl::ParseCastExpr() {
  Syntax::CastOperator cop = TokenToCastOp(CurrentToken().GetTokenKind());
  SourceRange castOpRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!ExpectAndConsume(Token::TK_SYM_LPAREN)) {
    return nullptr;
  }
  sona::owner<Syntax::Expr> castedExpr = ParseExpr();
  ExpectAndConsume(Token::TK_SYM_RPAREN);

  return new Syntax::CastExpr(cop, std::move(castedExpr), castOpRange);
}

sona::owner<Syntax::Expr> ParserImpl::ParsePostfixExpr() {
  sona::owner<Syntax::Expr> parsed = ParseUnaryExpr();

  for (;;) {
    switch (CurrentToken().GetTokenKind()) {
    case Token::TK_SYM_LPAREN:
      parsed = ParseFuncCallExpr(std::move(parsed)); break;
    case Token::TK_SYM_LBRACKET:
      parsed = ParseArraySubscriptExpr(std::move(parsed)); break;
    case Token::TK_SYM_DOT:
      parsed = ParseMemberAccessExpr(std::move(parsed)); break;
    default:
      return parsed;
    }
  }
}

sona::owner<Syntax::Expr>
ParserImpl::ParseFuncCallExpr(sona::owner<Syntax::Expr>&& parsedCallee) {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_SYM_LPAREN);
  ConsumeToken();

  std::vector<sona::owner<Syntax::Expr>> args;

  if (CurrentToken().GetTokenKind() == Token::TK_SYM_RPAREN) {
    ConsumeToken();
    return new Syntax::FuncCallExpr(std::move(parsedCallee), std::move(args));
  }

  for (;;) {
    args.push_back(ParseExpr());
    if (CurrentToken().GetTokenKind() == Token::TK_SYM_RPAREN) {
      ConsumeToken();
      break;
    }
    ExpectAndConsume(Token::TK_SYM_COMMA);
  }

  return new Syntax::FuncCallExpr(std::move(parsedCallee), std::move(args));
}

sona::owner<Syntax::Expr>
ParserImpl::ParseArraySubscriptExpr(sona::owner<Syntax::Expr>&& arr) {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_SYM_LBRACKET);
  ConsumeToken();

  sona::owner<Syntax::Expr> index = ParseExpr();
  ExpectAndConsume(Token::TK_SYM_RBRACKET);

  return new Syntax::ArraySubscriptExpr(std::move(arr), std::move(index));
}

sona::owner<Syntax::Expr>
ParserImpl::ParseMemberAccessExpr(sona::owner<Syntax::Expr>&& base) {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_SYM_DOT);
  ConsumeToken();
  sona::owner<Syntax::Identifier> member = ParseIdentifier();
  return new Syntax::MemberAccessExpr(std::move(base), std::move(member));
}

sona::owner<Syntax::Expr>
ParserImpl::ParseBinaryExpr(std::uint16_t prevPrec) {
  sona::owner<Syntax::Expr> e0 = ParseUnaryExpr();
  if (e0.borrow() == nullptr) {
    return nullptr;
  }

  Syntax::BinaryOperator op = TokenToBinary(CurrentToken().GetTokenKind());

  while (op != Syntax::BinaryOperator::BOP_Invalid
         && Syntax::PrecOf(op) >= prevPrec) {
    SourceRange opRange = CurrentToken().GetSourceRange();
    ConsumeToken();

    sona::owner<Syntax::Expr> e1 = ParseBinaryExpr(Syntax::PrecOf(op) + 1);
    e0 = new Syntax::BinaryExpr(op, std::move(e0), std::move(e1), opRange);
  }

  return e0;
}

owner<Syntax::Type> ParserImpl::ParseBuiltinType() {
  Syntax::BasicType::TypeKind kind;
  switch (CurrentToken().GetTokenKind()) {
  case Token::TK_KW_int8:
    kind = Syntax::BasicType::TypeKind::TK_Int8; break;
  case Token::TK_KW_int16:
    kind = Syntax::BasicType::TypeKind::TK_Int16; break;
  case Token::TK_KW_int32:
    kind = Syntax::BasicType::TypeKind::TK_Int32; break;
  case Token::TK_KW_int64:
    kind = Syntax::BasicType::TypeKind::TK_Int64; break;
  case Token::TK_KW_uint8:
    kind = Syntax::BasicType::TypeKind::TK_UInt8; break;
  case Token::TK_KW_uint16:
    kind = Syntax::BasicType::TypeKind::TK_UInt16; break;
  case Token::TK_KW_uint32:
    kind = Syntax::BasicType::TypeKind::TK_UInt32; break;
  case Token::TK_KW_uint64:
    kind = Syntax::BasicType::TypeKind::TK_UInt64; break;
  case Token::TK_KW_float:
    kind = Syntax::BasicType::TypeKind::TK_Float; break;
  case Token::TK_KW_double:
    kind = Syntax::BasicType::TypeKind::TK_Double; break;
  default:
    return nullptr;
  }

  SourceRange range = CurrentToken().GetSourceRange();
  ConsumeToken();

  return new Syntax::BasicType(kind, range);
}

owner<Syntax::Type> ParserImpl::ParseUserDefinedType() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_ID);
  string_ref typeStr = CurrentToken().GetStrValueUnsafe();
  SourceRange range = CurrentToken().GetSourceRange();
  ConsumeToken();
  return new Syntax::UserDefinedType(typeStr, range);
}

sona::owner<Syntax::Identifier> ParserImpl::ParseIdentifier() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_ID);

  std::vector<sona::string_ref> parsedParts;
  std::vector<SourceRange> parsedPartRanges;

  parsedParts.push_back(CurrentToken().GetStrValueUnsafe());
  parsedPartRanges.push_back(CurrentToken().GetSourceRange());

  for (;;) {
    if (CurrentToken().GetTokenKind() != Token::TK_SYM_DOT) {
      break;
    }
    ExpectAndConsume(Token::TK_SYM_DOT);
    if (!Expect(Token::TK_ID)) {
      ConsumeToken();
      continue;
    }

    parsedParts.push_back(CurrentToken().GetStrValueUnsafe());
    parsedPartRanges.push_back(CurrentToken().GetSourceRange());
    ConsumeToken();
  }

  sona::string_ref idItself = parsedParts.back();
  SourceRange idItselfRange = parsedPartRanges.back();
  parsedParts.pop_back();
  parsedPartRanges.pop_back();

  return new Syntax::Identifier(std::move(parsedParts),idItself,
                                std::move(parsedPartRanges), idItselfRange);
}

void ParserImpl::
SetParsingTokenStream(ref_ptr<std::vector<Token> const> tokenStream) {
  m_ParsingTokenStream = tokenStream;
  m_Index = 0;
}

Token const& ParserImpl::CurrentToken() const noexcept {
  return m_ParsingTokenStream.get()[m_Index];
}

Token const& ParserImpl::PeekToken(size_t peekCount) const noexcept {
  return m_ParsingTokenStream.get()[m_Index + peekCount];
}

void ParserImpl::ConsumeToken() noexcept {
  m_Index++;
}

bool ParserImpl::Expect(Token::TokenKind tokenKind) const noexcept {
  if (CurrentToken().GetTokenKind() == tokenKind) {
    return true;
  }

  m_Diag.Diag(Diag::DIR_Error,
              Diag::Format(Diag::DMT_ErrExpectedGot, {
                             PrettyPrintToken(CurrentToken()),
                             PrettyPrintTokenKind(tokenKind)
                           }),
              CurrentToken().GetSourceRange());

  return false;
}

bool ParserImpl::ExpectAndConsume(Token::TokenKind tokenKind) noexcept {
  bool got = Expect(tokenKind);
  if (got) {
    ConsumeToken();
  }
  return got;
}

string_ref ParserImpl::PrettyPrintTokenKind(Token::TokenKind tokenKind) const {
  switch (tokenKind) {
#define TOKEN_KWD(name, rep) \
  case Token::TK_KW_##name: return "'" + std::string(rep) + "'";
#define TOKEN_SYM(name, rep) \
  case Token::TK_SYM_##name: return "'" + std::string(rep) + "'";
#define TOKEN_MISC(name, desc) \
  case Token::TK_##name: return desc;
#include "Frontend/Tokens.def"
  case Token::TK_INVALID: sona_unreachable();
  }
  sona_unreachable();
  return "";
}

Syntax::BasicType::TypeKind
ParserImpl::EvaluateIntTypeKind(int64_t i) noexcept {
  if (i <= std::numeric_limits<int8_t>::max()
      && i >= std::numeric_limits<int8_t>::min()) {
    return Syntax::BasicType::TypeKind::TK_Int8;
  }
  else if (i < std::numeric_limits<int16_t>::max()
           && i >= std::numeric_limits<int16_t>::min()) {
    return Syntax::BasicType::TypeKind::TK_Int16;
  }
  else if (i < std::numeric_limits<int32_t>::max()
           && i >= std::numeric_limits<int32_t>::min()) {
    return Syntax::BasicType::TypeKind::TK_Int32;
  }
  else {
    return Syntax::BasicType::TypeKind::TK_Int64;
  }
}

Syntax::BasicType::TypeKind
ParserImpl::EvaluateUIntTypeKind(uint64_t u) noexcept {
  if (u <= std::numeric_limits<uint8_t>::max()
      && u >= std::numeric_limits<uint8_t>::min()) {
    return Syntax::BasicType::TypeKind::TK_UInt8;
  }
  else if (u < std::numeric_limits<uint16_t>::max()
           && u >= std::numeric_limits<uint16_t>::min()) {
    return Syntax::BasicType::TypeKind::TK_UInt16;
  }
  else if (u < std::numeric_limits<uint32_t>::max()
           && u >= std::numeric_limits<uint32_t>::min()) {
    return Syntax::BasicType::TypeKind::TK_UInt32;
  }
  else {
    return Syntax::BasicType::TypeKind::TK_UInt64;
  }
}

Syntax::BasicType::TypeKind
ParserImpl::EvaluateFloatTypeKind(double) noexcept {
  return Syntax::BasicType::TypeKind::TK_Double;
}

string_ref ParserImpl::PrettyPrintToken(Token const& token) const {
  switch (token.GetTokenKind()) {
#define TOKEN_KWD(name, rep) \
  case Token::TK_KW_##name: return "'" + std::string(rep) + "'";
#define TOKEN_SYM(name, rep) \
  case Token::TK_SYM_##name: return "'" + std::string(rep) + "'";
#include "Frontend/Tokens.def"

  case Token::TK_ID:
    return "identifier '" + token.GetStrValueUnsafe().get() + "'";
  case Token::TK_LIT_INT:
    return "intergral literal '"
           + std::to_string(token.GetIntValueUnsafe()) + "'";
  case Token::TK_LIT_UINT:
    return "unsigned literal '"
           + std::to_string(token.GetUIntValueUnsafe()) + "'";
  case Token::TK_LIT_STR:
    return "string literal \""
           + token.GetStrValueUnsafe().get() + "\"";
  case Token::TK_EOI:
    return "end of input";
  default:
    sona_unreachable();
  }

  return "";
}

void ParserImpl::SkipTo(Token::TokenKind tokenKind) {
  SkipToAnyOf({ tokenKind, Token::TK_EOI });
}

void ParserImpl::
SkipToAnyOf(const std::initializer_list<Token::TokenKind>& tokenKinds) {
  SkipUntil([this, &tokenKinds] {
    return std::find(tokenKinds.begin(), tokenKinds.end(),
                     CurrentToken().GetTokenKind()) != tokenKinds.end();
  });
}

template <typename Cond> void ParserImpl::SkipUntil(Cond cond) {
  while (!(cond())) {
    ConsumeToken();
  }
}

Syntax::UnaryOperator
TokenToUnary(Frontend::Token::TokenKind token) noexcept {
  using namespace Syntax;
  switch (token) {
  case Frontend::Token::TK_SYM_PLUS: return UnaryOperator::UOP_Positive;
  case Frontend::Token::TK_SYM_MINUS: return UnaryOperator::UOP_Negative;
  case Frontend::Token::TK_SYM_ASTER: return UnaryOperator::UOP_Deref;
  case Frontend::Token::TK_SYM_AMP: return UnaryOperator::UOP_PointerTo;

  default:
    sona_unreachable();
    return UnaryOperator::UOP_Positive; // For silencing compiler warnings
  }
}

Syntax::BinaryOperator
TokenToBinary(Frontend::Token::TokenKind token) noexcept {
  using namespace Syntax;
  switch (token) {
  case Frontend::Token::TK_SYM_PLUS:  return BinaryOperator::BOP_Add;
  case Frontend::Token::TK_SYM_MINUS: return BinaryOperator::BOP_Sub;
  case Frontend::Token::TK_SYM_ASTER: return BinaryOperator::BOP_Mul;

  default:
    sona_unreachable();
    return BinaryOperator::BOP_Invalid;
  }
}

Syntax::CastOperator
TokenToCastOp(Frontend::Token::TokenKind token) noexcept {
  using namespace Syntax;
  switch (token) {
  case Frontend::Token::TK_KW_static_cast: return CastOperator::COP_StaticCast;
  case Frontend::Token::TK_KW_bitcast: return CastOperator::COP_BitCast;
  case Frontend::Token::TK_KW_const_cast: return CastOperator::COP_ConstCast;

  default:
    sona_unreachable();
    return CastOperator::COP_BitCast;
  }
}

} // namespace Frontend
} // namespace ckx
