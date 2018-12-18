#include "Frontend/Parser.h"

using namespace sona;

namespace ckx {
namespace Frontend {

owner<Syntax::TransUnit>
Parser::ParseTransUnit(ref_ptr<std::vector<Token> const> tokenStream) {
  m_ParsingTokenStream = tokenStream;
  return nullptr;
}

owner<Syntax::Stmt>
Parser::ParseLine(sona::ref_ptr<std::vector<Token> const> tokenStream) {
  m_ParsingTokenStream = tokenStream;
  return nullptr;
}

owner<Syntax::Decl> Parser::ParseDeclOrFndef() {
  switch (CurrentToken().GetTokenKind()) {
  case Token::TK_KW_def: return ParseVarDecl();
  case Token::TK_KW_class:
  case Token::TK_KW_enum:
  case Token::TK_KW_func:
    sona_unreachable1("not implemented");
  default:
    sona_unreachable();
  }
  return nullptr;
}

owner<Syntax::Decl> Parser::ParseVarDecl() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_def);
  SourceRange defRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  std::fprintf(stderr, "def parsed\n");

  if (!Expect(Token::TK_ID)) {
    /// add proper skipping
    return nullptr;
  }

  sona::string_ref name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  std::fprintf(stderr, "name parsed\n");

  ExpectAndConsume(Token::TK_SYM_COLON);

  std::fprintf(stderr, "colon parsed\n");

  owner<Syntax::Type> type = ParseType();

  std::fprintf(stderr, "type parsed\n");

  return new Syntax::VarDecl(name, std::move(type), defRange, nameRange);
}

owner<Syntax::Type> Parser::ParseType() {
  owner<Syntax::Type> ret = nullptr;
  if (CurrentToken().GetTokenKind() == Token::TK_ID) {
    ret = ParseUserDefinedType();
  }
  else {
    ret = ParseBuiltinType();
  }

  if (ret.borrow() == nullptr) {
    m_Diag.Diag(Diag::DiagnosticEngine::DIR_Error,
                Diag::Format(Diag::DMT_ErrExpectedGot, {
                               PrettyPrintToken(CurrentToken()),
                               "type"
                             }),
                CurrentToken().GetSourceRange());
  }

  return ret;
}

owner<Syntax::Type> Parser::ParseBuiltinType() {
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

owner<Syntax::Type> Parser::ParseUserDefinedType() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_ID);
  string_ref typeStr = CurrentToken().GetStrValueUnsafe();
  SourceRange range = CurrentToken().GetSourceRange();
  ConsumeToken();
  return new Syntax::UserDefinedType(typeStr, range);
}

void
Parser::SetParsingTokenStream(ref_ptr<std::vector<Token> const> tokenStream) {
  m_ParsingTokenStream = tokenStream;
}

Token const& Parser::CurrentToken() const noexcept {
  return m_ParsingTokenStream.get()[m_Index];
}

Token const& Parser::PeekToken(size_t peekCount) const noexcept {
  return m_ParsingTokenStream.get()[peekCount];
}

void Parser::ConsumeToken() noexcept {
  m_Index++;
}

bool Parser::Expect(Token::TokenKind tokenKind) const noexcept {
  if (CurrentToken().GetTokenKind() == tokenKind) {
    return true;
  }

  m_Diag.Diag(Diag::DiagnosticEngine::DIR_Error,
              Diag::Format(Diag::DMT_ErrExpectedGot, {
                             PrettyPrintToken(CurrentToken()),
                             PrettyPrintTokenKind(tokenKind)
                           }),
              CurrentToken().GetSourceRange());

  return false;
}

bool Parser::ExpectAndConsume(Token::TokenKind tokenKind) noexcept {
  bool got = Expect(tokenKind);
  if (got) {
    ConsumeToken();
  }
  return got;
}

string_ref Parser::PrettyPrintTokenKind(Token::TokenKind tokenKind) const {
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

string_ref Parser::PrettyPrintToken(Token const& token) const {
  switch (token.GetTokenKind()) {
#define TOKEN_KWD(name, rep) \
  case Token::TK_KW_##name: return "'" + std::string(rep) + "'";
#define TOKEN_SYM(name, rep) \
  case Token::TK_SYM_##name: return "'" + std::string(rep) + "'";
#include "Frontend/Tokens.def"

  case Token::TK_ID: return "identifier " + token.GetStrValueUnsafe().get();
  case Token::TK_LIT_INT:
    return "intergral literal " + std::to_string(token.GetIntValueUnsafe());
  case Token::TK_LIT_UINT:
    return "unsigned literal " + std::to_string(token.GetUIntValueUnsafe());
  case Token::TK_LIT_STR:
    return "string literal " + token.GetStrValueUnsafe().get();
  case Token::TK_EOI:
    return "end of input";
  default:
    sona_unreachable();
  }

  return "";
}

} // namespace Frontend
} // namespace ckx
