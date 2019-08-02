#include "Frontend/ParserImpl.h"
#include "sona/global_counter.h"

namespace ckx {
namespace Frontend {

#define FIRST_DECLS Token::TK_EOI

static std::string CreateAnonymousName() {
  return "<anonymous id=\"" + std::to_string(sona::global_count()) + "\"/>";
}

sona::owner<Syntax::TransUnit>
ParserImpl::ParseTransUnit(
    sona::ref_ptr<std::vector<Token> const> tokenStream) {
  SetParsingTokenStream(tokenStream);

  sona::owner<Syntax::TransUnit> ret = new Syntax::TransUnit;
  while (CurrentToken().GetTokenKind() != Token::TK_EOI) {
    sona::owner<Syntax::Decl> d = ParseDeclOrFndef();
    if (d.borrow() == nullptr) {
      continue;
    }
    if (d.borrow()->GetNodeKind() == Syntax::Node::CNK_VarDecl) {
      ExpectAndConsume(Token::TK_SYM_SEMI);
    }
    ret.borrow()->Declare(std::move(d));
  }

  return ret;
}

sona::owner<Syntax::Stmt>
ParserImpl::ParseLine(sona::ref_ptr<std::vector<Token> const> tokenStream) {
  SetParsingTokenStream(tokenStream);
  return nullptr;
}

sona::owner<Syntax::Decl> ParserImpl::ParseDeclOrFndef() {
  switch (CurrentToken().GetTokenKind()) {
  case Token::TK_KW_def: return ParseVarDecl();
  case Token::TK_KW_class: return ParseClassDecl();
  case Token::TK_KW_using: return ParseUsingDecl();
  case Token::TK_KW_enum:
    if (PeekToken().GetTokenKind() == Token::TK_KW_class) {
      return ParseADTDecl();
    } else {
      return ParseEnumDecl();
    }
  case Token::TK_KW_func: return ParseFuncDecl();

  default:
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(
                  Diag::DMT_ErrExpectedGot,
                  { PrettyPrintTokenKind(CurrentToken().GetTokenKind()),
                    "declaration" }),
                CurrentToken().GetSourceRange());
    ConsumeToken();
    return nullptr;
  }
}

sona::owner<Syntax::Decl> ParserImpl::ParseVarDecl() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_def);
  SourceRange defRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!Expect(Token::TK_ID)) {
    return nullptr;
  }

  sona::strhdl_t name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  ExpectAndConsume(Token::TK_SYM_COLON);

  sona::owner<Syntax::Type> type = ParseType();
  return new Syntax::VarDecl(name, std::move(type), defRange, nameRange);
}

sona::owner<Syntax::Decl> ParserImpl::ParseClassDecl() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_class);
  SourceRange classRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  sona::optional<std::pair<sona::strhdl_t, SourceRange>>
  idResult = ExpectTagId();
  if (!idResult.has_value()) {
    return nullptr;
  }
  sona::strhdl_t name = idResult.value().first;
  SourceRange nameRange = idResult.value().second;

  if (!ExpectAndConsume(Token::TK_SYM_LBRACE)) {
    if (CurrentToken().GetTokenKind() == Token::TK_SYM_SEMI) {
      m_Diag.Diag(Diag::DIR_Note,
                  Diag::Format(Diag::DMT_NoteNoForwardDecl, {}),
                  CurrentToken().GetSourceRange());
      ConsumeToken();
      return new Syntax::ClassDecl(name, {}, classRange, nameRange);
    }
    else {
      return nullptr;
    }
  }

  std::vector<sona::owner<Syntax::Decl>> decls;

  while (CurrentToken().GetTokenKind() != Token::TK_EOI
         && CurrentToken().GetTokenKind() != Token::TK_SYM_RBRACE) {
    switch (CurrentToken().GetTokenKind()) {
    case Token::TK_KW_def:
      decls.push_back(ParseVarDecl());
      ExpectAndConsume(Token::TK_SYM_SEMI);
      break;
    case Token::TK_KW_class: decls.push_back(ParseClassDecl()); break;
    case Token::TK_KW_enum:
      if (PeekToken().GetTokenKind() == Token::TK_KW_class) {
        decls.push_back(ParseADTDecl());
      }
      else {
        decls.push_back(ParseEnumDecl());
      }
      break;
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

sona::owner<Syntax::Decl> ParserImpl::ParseEnumDecl() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_enum);
  SourceRange enumRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  sona::optional<std::pair<sona::strhdl_t, SourceRange>>
  idResult = ExpectTagId();
  if (!idResult.has_value()) {
    return nullptr;
  }
  sona::strhdl_t name = idResult.value().first;
  SourceRange nameRange = idResult.value().second;

  if (!ExpectAndConsume(Token::TK_SYM_LBRACE)) {
    if (CurrentToken().GetTokenKind() == Token::TK_SYM_SEMI) {
      m_Diag.Diag(Diag::DIR_Note,
                  Diag::Format(Diag::DMT_NoteNoForwardDecl, {}),
                  CurrentToken().GetSourceRange());
      ConsumeToken();
      return new Syntax::EnumDecl(name, {}, enumRange, nameRange);
    }
    else {
      return nullptr;
    }
  }

  std::vector<Syntax::EnumDecl::Enumerator> enumerators;

  while (CurrentToken().GetTokenKind() != Token::TK_EOI
         && CurrentToken().GetTokenKind() != Token::TK_SYM_RBRACE) {
    if (CurrentToken().GetTokenKind() != Token::TK_ID) {
      SkipToAnyOf({ Token::TK_ID, Token::TK_SYM_SEMI, Token::TK_SYM_RBRACE });
      if (CurrentToken().GetTokenKind() == Token::TK_SYM_SEMI) {
        ConsumeToken();
        continue;
      }
    }
    ParseEnumerator(enumerators);
    if (!ExpectAndConsume(Token::TK_SYM_SEMI)) {
      if (CurrentToken().GetTokenKind() == Token::TK_SYM_COMMA) {
        m_Diag.Diag(Diag::DIR_Note,
                    Diag::Format(Diag::DMT_NoteEnumeratorSep, {}),
                    CurrentToken().GetSourceRange());
        ConsumeToken();
      }
    }
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

  sona::optional<std::pair<sona::strhdl_t, SourceRange>>
  idResult = ExpectTagId();
  if (!idResult.has_value()) {
    return nullptr;
  }
  sona::strhdl_t name = idResult.value().first;
  SourceRange nameRange = idResult.value().second;

  if (!ExpectAndConsume(Token::TK_SYM_LBRACE)) {
    if (CurrentToken().GetTokenKind() == Token::TK_SYM_SEMI) {
      m_Diag.Diag(Diag::DIR_Note,
                  Diag::Format(Diag::DMT_NoteNoForwardDecl, {}),
                  CurrentToken().GetSourceRange());
      ConsumeToken();
      return new Syntax::ADTDecl(name, {}, enumRange, classRange, nameRange);
    }
    else {
      return nullptr;
    }
  }

  std::vector<Syntax::ADTDecl::ValueConstructor> dataConstructors;

  while (CurrentToken().GetTokenKind() != Token::TK_EOI
         && CurrentToken().GetTokenKind() != Token::TK_SYM_RBRACE) {
    if (CurrentToken().GetTokenKind() != Token::TK_ID) {
      SkipToAnyOf({ Token::TK_ID, Token::TK_SYM_SEMI, Token::TK_SYM_RBRACE });
      if (CurrentToken().GetTokenKind() == Token::TK_SYM_SEMI) {
        ConsumeToken();
        continue;
      }
    }
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

  sona::strhdl_t name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!ExpectAndConsume(Token::TK_SYM_LPAREN)) {
    return nullptr;
  }

  std::vector<sona::strhdl_t> paramNames;
  std::vector<sona::owner<Syntax::Type>> paramTypes;

  while (CurrentToken().GetTokenKind() != Token::TK_EOI) {
    if (!Expect(Token::TK_ID)) {
      continue;
    }
    sona::strhdl_t paramName = CurrentToken().GetStrValueUnsafe();
    ConsumeToken();

    if (!ExpectAndConsume(Token::TK_SYM_COLON)) {
      continue;
    }

    sona::owner<Syntax::Type> type = ParseType();
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

  sona::owner<Syntax::Type> retType = ParseType();
  ExpectAndConsume(Token::TK_SYM_SEMI);
  return new Syntax::FuncDecl(name, std::move(paramTypes),
                              std::move(paramNames), std::move(retType),
                              sona::empty_optional(), funcRange, nameRange);
}

sona::owner<Syntax::Decl> ParserImpl::ParseUsingDecl() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_KW_using);
  SourceRange usingRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!Expect(Token::TK_ID)) {
    return nullptr;
  }
  sona::strhdl_t name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!Expect(Token::TK_SYM_EQ)) {
    return nullptr;
  }
  SourceLocation eqLoc =
      SourceLocation(CurrentToken().GetSourceRange().GetStartLine(),
                     CurrentToken().GetSourceRange().GetStartCol());
  ConsumeToken();

  sona::owner<Syntax::Type> aliasee = ParseType();
  ExpectAndConsume(Token::TK_SYM_SEMI);

  return new Syntax::UsingDecl(name, std::move(aliasee), usingRange,
                               nameRange, eqLoc);
}

void ParserImpl::
ParseEnumerator(std::vector<Syntax::EnumDecl::Enumerator> &enumerators) {
  if (!Expect(Token::TK_ID)) {
    return;
  }

  sona::strhdl_t name = CurrentToken().GetStrValueUnsafe();
  SourceRange nameRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (CurrentToken().GetTokenKind() != Token::TK_SYM_EQ) {
    enumerators.emplace_back(name, nameRange);
    return;
  }

  SourceRange eqRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  if (!Expect(Token::TK_LIT_INT)) {
    SkipToAnyOf({ Token::TK_ID, Token::TK_SYM_COMMA, Token::TK_SYM_RBRACE });
  }

  int64_t value = CurrentToken().GetIntValueUnsafe();
  SourceRange valueRange = CurrentToken().GetSourceRange();
  ConsumeToken();

  enumerators.emplace_back(name, value, nameRange, eqRange, valueRange);
}

void ParserImpl::ParseDataConstructor(
  std::vector<Syntax::ADTDecl::ValueConstructor> &dataConstructors) {
  if (!Expect(Token::TK_ID)) {
    return;
  }

  sona::strhdl_t name = CurrentToken().GetStrValueUnsafe();
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
  if (!ExpectAndConsume(Token::TK_SYM_RPAREN)) {
    if (CurrentToken().GetTokenKind() == Token::TK_SYM_COMMA) {
      m_Diag.Diag(Diag::DIR_Note,
                  Diag::Format(Diag::DMT_NoteOneTypeInValueCtor, {}),
                  CurrentToken().GetSourceRange());
      dataConstructors.emplace_back(name, std::move(underlyingType), nameRange);
    }

    SkipToAnyOf({Token::TK_SYM_RPAREN, Token::TK_SYM_RBRACE,
                 Token::TK_ID, Token::TK_SYM_SEMI});
    if (CurrentToken().GetTokenKind() == Token::TK_SYM_RPAREN) {
      ConsumeToken();
    }
    return;
  }

  dataConstructors.emplace_back(name, std::move(underlyingType), nameRange);
}

sona::owner<Syntax::Type> ParserImpl::ParseType() {
  sona::owner<Syntax::Type> ret = nullptr;
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

  std::vector<Syntax::ComposedType::TypeSpecifier> tySpecs;
  std::vector<SourceRange> tySpecRanges;
  while (CurrentToken().GetTokenKind() == Token::TK_SYM_AMP
         || CurrentToken().GetTokenKind() == Token::TK_SYM_DAMP
         || CurrentToken().GetTokenKind() == Token::TK_SYM_ASTER
         || CurrentToken().GetTokenKind() == Token::TK_KW_const
         || CurrentToken().GetTokenKind() == Token::TK_KW_volatile
         || CurrentToken().GetTokenKind() == Token::TK_KW_restrict) {
    switch (CurrentToken().GetTokenKind()) {
    case Token::TK_SYM_AMP:
      tySpecs.push_back(Syntax::ComposedType::CTS_Ref);
      break;
    case Token::TK_SYM_DAMP:
      tySpecs.push_back(Syntax::ComposedType::CTS_RvRef);
      break;
    case Token::TK_SYM_ASTER:
      tySpecs.push_back(Syntax::ComposedType::CTS_Pointer);
      break;
    case Token::TK_KW_const:
      tySpecs.push_back(Syntax::ComposedType::CTS_Const);
      break;
    case Token::TK_KW_volatile:
      tySpecs.push_back(Syntax::ComposedType::CTS_Volatile);
      break;
    case Token::TK_KW_restrict:
      tySpecs.push_back(Syntax::ComposedType::CTS_Restrict);
      break;

    default:
      sona_unreachable();
    }
    tySpecRanges.push_back(CurrentToken().GetSourceRange());
    ConsumeToken();
  }

  if (tySpecs.size() != 0) {
    sona_assert(tySpecs.size() == tySpecRanges.size())
    ret = new Syntax::ComposedType(std::move(ret), std::move(tySpecs),
                                   std::move(tySpecRanges));
  }

  return ret;
}

sona::owner<Syntax::Expr> ParserImpl::ParseExpr() {
  /// @todo this needs a bit fix to work. BianryOperator now does not contain
  /// assignment operator.
  return ParseBinaryExpr(Syntax::PrecOf(Syntax::BinaryOperator::BOP_Eq));
}

sona::owner<Syntax::Expr> ParserImpl::ParseLiteralExpr() {
  sona::owner<Syntax::Expr> ret = nullptr;
  switch (CurrentToken().GetTokenKind()) {
  case Token::TK_KW_true:
    ret = new Syntax::BoolLiteralExpr(true, CurrentToken().GetSourceRange());
    break;

  case Token::TK_KW_false:
    ret = new Syntax::BoolLiteralExpr(false, CurrentToken().GetSourceRange());
    break;

  case Token::TK_KW_nullptr:
    ret = new Syntax::NullLiteralExpr(CurrentToken().GetSourceRange());
    break;

  case Token::TK_LIT_INT:
    ret = new Syntax::IntLiteralExpr(
            CurrentToken().GetIntValueUnsafe(),
            CurrentToken().GetSourceRange());
    break;
  case Token::TK_LIT_UINT:
    ret = new Syntax::UIntLiteralExpr(
            CurrentToken().GetUIntValueUnsafe(),
            CurrentToken().GetSourceRange());
    break;
  case Token::TK_LIT_FLOAT:
    ret = new Syntax::FloatLiteralExpr(
            CurrentToken().GetFloatValueUnsafe(),
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

  if (!ExpectAndConsume(Token::TK_SYM_LT)) {
    return nullptr;
  }
  sona::owner<Syntax::Type> destType = ParseType();
  ExpectAndConsume(Token::TK_SYM_GT);

  if (!ExpectAndConsume(Token::TK_SYM_LPAREN)) {
    return nullptr;
  }
  sona::owner<Syntax::Expr> castedExpr = ParseExpr();
  ExpectAndConsume(Token::TK_SYM_RPAREN);

  return new Syntax::CastExpr(cop, std::move(castedExpr),
                              std::move(destType), castOpRange);
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
  Syntax::Identifier member = ParseIdentifier();
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

sona::owner<Syntax::Type> ParserImpl::ParseBuiltinType() {
  Syntax::BuiltinType::BuiltinTypeId btid;
  switch (CurrentToken().GetTokenKind()) {
  #define BUILTIN_TYPE(name, size, isint, \
                       issigned, signedver, unsignedver, token) \
    case Frontend::Token::token: \
      btid = Syntax::BuiltinType::TK_##name; break;
  #include "Syntax/BuiltinTypes.def"

  default:
    sona_unreachable();
    return nullptr;
  }

  SourceRange range = CurrentToken().GetSourceRange();
  ConsumeToken();

  return new Syntax::BuiltinType(btid, range);
}

sona::owner<Syntax::Type> ParserImpl::ParseUserDefinedType() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_ID);
  Syntax::Identifier id = ParseIdentifier();
  return new Syntax::UserDefinedType(std::move(id), id.GetIdSourceRange());
}

Syntax::Identifier ParserImpl::ParseIdentifier() {
  sona_assert(CurrentToken().GetTokenKind() == Token::TK_ID);

  std::vector<sona::strhdl_t> parsedParts;
  std::vector<SourceRange> parsedPartRanges;

  parsedParts.push_back(CurrentToken().GetStrValueUnsafe());
  parsedPartRanges.push_back(CurrentToken().GetSourceRange());
  ConsumeToken();

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

  sona::strhdl_t idItself = parsedParts.back();
  SourceRange idItselfRange = parsedPartRanges.back();
  parsedParts.pop_back();
  parsedPartRanges.pop_back();

  return Syntax::Identifier(std::move(parsedParts),idItself,
                            std::move(parsedPartRanges), idItselfRange);
}

void ParserImpl::
SetParsingTokenStream(sona::ref_ptr<std::vector<Token> const> tokenStream) {
  m_ParsingTokenStream = tokenStream;
  m_Index = 0;
}

sona::optional<std::pair<sona::strhdl_t, SourceRange>>
ParserImpl::ExpectTagId() {
  if (!Expect(Token::TK_ID)) {
    if (CurrentToken().GetTokenKind() == Token::TK_SYM_LBRACE) {
      m_Diag.Diag(Diag::DIR_Note,
                  Diag::Format(Diag::DMT_NoteNoAnonymousDecl, {}),
                  CurrentToken().GetSourceRange());
      sona::strhdl_t name = CreateAnonymousName();
      SourceRange range = CurrentToken().GetSourceRange();
      return std::make_pair(name, range);
    }
    else {
      return sona::empty_optional();
    }
  }

  sona::strhdl_t name = CurrentToken().GetStrValueUnsafe();
  SourceRange range = CurrentToken().GetSourceRange();
  ConsumeToken();
  return std::make_pair(name, range);
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

sona::strhdl_t ParserImpl::PrettyPrintToken(Token const& token) const {
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
  SkipToAnyOf({ tokenKind  });
}

void ParserImpl::
SkipToAnyOf(const std::initializer_list<Token::TokenKind>& tokenKinds) {
  SkipUntil([this, &tokenKinds] {
    return (std::find(tokenKinds.begin(), tokenKinds.end(),
                     CurrentToken().GetTokenKind()) != tokenKinds.end())
           || (CurrentToken().GetTokenKind() == Token::TK_EOI);
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
  case Frontend::Token::TK_SYM_PLUS:    return UnaryOperator::UOP_Positive;
  case Frontend::Token::TK_SYM_MINUS:   return UnaryOperator::UOP_Negative;
  case Frontend::Token::TK_SYM_ASTER:   return UnaryOperator::UOP_Deref;
  case Frontend::Token::TK_SYM_AMP:     return UnaryOperator::UOP_AddrOf;
  case Frontend::Token::TK_SYM_DPLUS:   return UnaryOperator::UOP_SelfIncr;
  case Frontend::Token::TK_SYM_DMINUS:  return UnaryOperator::UOP_SelfDecr;
  case Frontend::Token::TK_SYM_EXCLAIM: return UnaryOperator::UOP_LogicNot;
  case Frontend::Token::TK_SYM_WAVE:    return UnaryOperator::UOP_BitReverse;

  default: ;
  }
  sona_unreachable();
  return UnaryOperator::UOP_Positive; // For silencing compiler warnings
}

Syntax::BinaryOperator
TokenToBinary(Frontend::Token::TokenKind token) noexcept {
  using namespace Syntax;
  switch (token) {
  case Frontend::Token::TK_SYM_PLUS:  return BinaryOperator::BOP_Add;
  case Frontend::Token::TK_SYM_MINUS: return BinaryOperator::BOP_Sub;
  case Frontend::Token::TK_SYM_ASTER: return BinaryOperator::BOP_Mul;

  default: ;
  }
  sona_unreachable();
  return BinaryOperator::BOP_Invalid;
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
