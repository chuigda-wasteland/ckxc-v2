#include "Sema/SemaPhase1.h"

#include "AST/Expr.h"
#include "Syntax/Concrete.h"

namespace ckx {
namespace Sema {

SemaPhase1::SemaPhase1(AST::ASTContext &astContext,
    std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
    Diag::DiagnosticEngine &diag)
  : SemaCommon(astContext, declContexts, diag) {}

void SemaPhase1::PostTranslateIncompletes(
    std::vector<sona::ref_ptr<IncompleteDecl>> incompletes) {
  for (auto incomplete : incompletes) {
    switch (incomplete->GetType()) {
    case IncompleteDecl::IDT_Var:
      PostTranslateIncompleteVar(incomplete.cast_unsafe<IncompleteVarDecl>());
      break;
    case IncompleteDecl::IDT_Tag:
      PostTranslateIncompleteTag(incomplete.cast_unsafe<IncompleteTagDecl>());
      break;
    case IncompleteDecl::IDT_ValueCtor:
      PostTranslateIncompleteValueCtor(
            incomplete.cast_unsafe<IncompleteValueCtorDecl>());
      break;
    case IncompleteDecl::IDT_Using:
      PostTranslateIncompleteUsing(
            incomplete.cast_unsafe<IncompleteUsingDecl>());
      break;
    case IncompleteDecl::IDT_Function:
      sona_unreachable1("functions should not be solved here");
      break;
    }
  }
}

void SemaPhase1::PostTranslateIncompleteVar(
    sona::ref_ptr<IncompleteVarDecl> iVar) {
  AST::QualType varType =
      ResolveType(iVar->GetEnclosingScope(), iVar->GetConcrete()->GetType());
  sona_assert(varType.GetUnqualTy() != nullptr);
  iVar->GetIncomplete()->SetType(varType);
}

void SemaPhase1::PostTranslateIncompleteTag(sona::ref_ptr<IncompleteTagDecl>) {
  // do nothing since the tag will be completed as its fields get completed
}

void SemaPhase1::PostTranslateIncompleteValueCtor(
    sona::ref_ptr<IncompleteValueCtorDecl> iValueCtor) {
  sona::ref_ptr<AST::ValueCtorDecl> halfway =
      iValueCtor->GetHalfway().cast_unsafe<AST::ValueCtorDecl>();
  AST::QualType adtConstructorType =
      ResolveType(iValueCtor->GetEnclosingScope(),
                  iValueCtor->GetConcrete()->GetUnderlyingType());
  halfway->SetType(adtConstructorType);
}

void SemaPhase1::PostTranslateIncompleteUsing(
    sona::ref_ptr<IncompleteUsingDecl> iusing) {
  AST::QualType aliasee = ResolveType(iusing->GetEnclosingScope(),
                                      iusing->GetConcrete()->GetAliasee());
  iusing->GetHalfway()->FillAliasee(aliasee);
}

AST::QualType
SemaPhase1::ResolveType(std::shared_ptr<Scope> scope,
                        sona::ref_ptr<const Syntax::Type> type) {
  switch (type->GetNodeKind()) {
#define CST_TYPE(name) \
  case Syntax::Node::CNK_##name: \
    return Resolve##name(scope, type.cast_unsafe<Syntax::name const>());
#include "Syntax/Nodes.def"
  default:
    sona_unreachable();
  }
  return sona::ref_ptr<AST::Type const>(nullptr);
}

sona::owner<AST::Expr>
SemaPhase1::ActOnExpr(std::shared_ptr<Scope> scope,
                      sona::ref_ptr<const Syntax::Expr> expr) {
  switch (expr->GetNodeKind()) {
#define CST_EXPR(name) \
  case Syntax::Node::CNK_##name: \
    return ActOn##name(scope, expr.cast_unsafe<Syntax::name const>());
#include "Syntax/Nodes.def"
  default:
    sona_unreachable();
  }
  return nullptr;
}

std::int8_t SemaPhase1::SIntRank(AST::BuiltinType::BuiltinTypeId btid) {
  return AST::BuiltinType::SIntRank(btid);
}

std::int8_t SemaPhase1::UIntRank(AST::BuiltinType::BuiltinTypeId btid) {
  return AST::BuiltinType::UIntRank(btid);
}

std::int8_t SemaPhase1::FloatRank(AST::BuiltinType::BuiltinTypeId btid) {
  return AST::BuiltinType::FloatRank(btid);
}

} // namespace Sema
} // namespace ckx
