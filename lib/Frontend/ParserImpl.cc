#include "Frontend/ParserImpl.h"

using namespace sona;

namespace ckx {
namespace Frontend {

owner<Syntax::TransUnit>
ParserImpl::ParseTransUnit(ref_ptr<std::vector<Token> const> tokenStream) {
  SetParsingTokenStream(tokenStream);
  return nullptr;
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
  case Token::TK_KW_enum: return ParseEnumDecl();
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

void
ParserImpl::SetParsingTokenStream(ref_ptr<std::vector<Token> const> tokenStream) {
  m_ParsingTokenStream = tokenStream;
  m_Index = 0;
}

Token const& ParserImpl::CurrentToken() const noexcept {
  return m_ParsingTokenStream.get()[m_Index];
}

Token const& ParserImpl::PeekToken(size_t peekCount) const noexcept {
  return m_ParsingTokenStream.get()[peekCount];
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
  static sona::string_ref emptyStrRef("");
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
  return emptyStrRef;
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

void
ParserImpl::SkipToAnyOf(const std::initializer_list<Token::TokenKind>& tokenKinds) {
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

} // namespace Frontend
} // namespace ckx
