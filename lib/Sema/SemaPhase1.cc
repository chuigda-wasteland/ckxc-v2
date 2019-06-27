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
      PostTranslateIncompleteADTConstructor(
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

AST::QualType SemaPhase1::ResolveType(std::shared_ptr<Scope> scope,
                        sona::ref_ptr<const Syntax::Type> type) {
  switch (type->GetNodeKind()) {
#define CST_TYPE(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return Resolve##name(scope, type.cast_unsafe<Syntax::name const>());
#include "Syntax/Nodes.def"
  default:
    sona_unreachable();
  }
  return sona::ref_ptr<AST::Type const>(nullptr);
}

void SemaPhase1::PostTranslateIncompleteVar(
    sona::ref_ptr<IncompleteVarDecl> iVar) {
  AST::QualType varType =
      ResolveType(iVar->GetEnclosingScope(), iVar->GetConcrete()->GetType());
  sona_assert(varType.GetUnqualTy() != nullptr);
  iVar->GetIncomplete()->SetType(varType);
}

void SemaPhase1::PostTranslateIncompleteTag(
    sona::ref_ptr<IncompleteTagDecl> iTag) {
  (void)iTag;
  // do nothing since the tag will be completed as its fields get completed
}

void SemaPhase1::PostTranslateIncompleteADTConstructor(
    sona::ref_ptr<IncompleteValueCtorDecl> iAdtC) {
  sona::ref_ptr<AST::ValueCtorDecl> halfway =
      iAdtC->GetHalfway().cast_unsafe<AST::ValueCtorDecl>();
  AST::QualType adtConstructorType =
      ResolveType(iAdtC->GetEnclosingScope(),
                  iAdtC->GetConcrete()->GetUnderlyingType());
  halfway->SetType(adtConstructorType);
}

void SemaPhase1::PostTranslateIncompleteUsing(
    sona::ref_ptr<IncompleteUsingDecl> iusing) {
  AST::QualType aliasee = ResolveType(iusing->GetEnclosingScope(),
                                      iusing->GetConcrete()->GetAliasee());
  iusing->GetHalfway()->FillAliasee(aliasee);
}

AST::QualType
SemaPhase1::ResolveBuiltinType(std::shared_ptr<Scope>,
                             sona::ref_ptr<Syntax::BuiltinType const> bty) {
  return SemaCommon::ResolveBuiltinTypeImpl(bty);
}

AST::QualType
SemaPhase1::
ResolveUserDefinedType(std::shared_ptr<Scope> scope,
                       sona::ref_ptr<Syntax::UserDefinedType const> uty) {
  AST::QualType lookupResult = LookupType(scope, uty->GetName(), false);
  sona_assert(lookupResult.GetUnqualTy() != nullptr);
  return lookupResult;
}

AST::QualType
SemaPhase1:: ResolveTemplatedType(std::shared_ptr<Scope>,
                                  sona::ref_ptr<Syntax::TemplatedType const>) {
  sona_unreachable1("not implemented");
  return sona::ref_ptr<AST::Type const>(nullptr);
}

AST::QualType
SemaPhase1::ResolveComposedType(std::shared_ptr<Scope> scope,
                                sona::ref_ptr<Syntax::ComposedType const> cty) {
  AST::QualType ret = ResolveType(scope, cty->GetRootType());

  /// @todo duplicate codes, remove them at some time.
  auto r = sona::linq::from_container(cty->GetTypeSpecifiers())
            .zip_with(
             sona::linq::from_container(cty->GetTypeSpecRanges()));
  for (const auto& p : r) {
    switch (p.first) {
    case Syntax::ComposedType::TypeSpecifier::CTS_Pointer:
      ret = m_ASTContext.CreatePointerType(ret);
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_Ref:
      ret = m_ASTContext.CreateLValueRefType(ret);
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_RvRef:
      ret = m_ASTContext.CreateRValueRefType(ret);
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_Const:
      if (ret.IsConst()) {
        m_Diag.Diag(Diag::DIR_Error,
                    Diag::Format(Diag::DMT_ErrDuplicateQual, {"const"}),
                    p.second);
      }
      else {
        ret.AddConst();
      }
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_Volatile:
      if (ret.IsVolatile()) {
        m_Diag.Diag(Diag::DIR_Error,
                    Diag::Format(Diag::DMT_ErrDuplicateQual, {"volatile"}),
                    p.second);
      }
      else {
        ret.AddVolatile();
      }
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_Restrict:
      if (ret.IsRestrict()) {
        m_Diag.Diag(Diag::DIR_Error,
                    Diag::Format(Diag::DMT_ErrDuplicateQual, {"restrict"}),
                    p.second);
      }
      else {
        ret.AddRestrict();
      }
      break;
    default:
      sona_unreachable1("not implemented");
    }
  }
  return ret;
}

sona::ref_ptr<AST::Expr const>
SemaPhase1::ActOnIntLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::IntLiteralExpr const> literalExpr) {
  AST::BuiltinType::BuiltinTypeId btid =
      ClassifyBuiltinTypeId(literalExpr->GetValue());
  return new AST::IntLiteralExpr(literalExpr->GetValue(),
                                 m_ASTContext.GetBuiltinType(btid));
}

sona::ref_ptr<AST::Expr const>
SemaPhase1::ActOnUIntLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::UIntLiteralExpr const> literalExpr) {
  AST::BuiltinType::BuiltinTypeId btid =
      ClassifyBuiltinTypeId(literalExpr->GetValue());
  return new AST::UIntLiteralExpr(literalExpr->GetValue(),
                                  m_ASTContext.GetBuiltinType(btid));
}

sona::ref_ptr<AST::Expr const>
SemaPhase1::ActOnFloatLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::FloatLiteralExpr const> literalExpr) {
  AST::BuiltinType::BuiltinTypeId btid =
      ClassifyBuiltinTypeId(literalExpr->GetValue());
  return new AST::FloatLiteralExpr(literalExpr->GetValue(),
                                   m_ASTContext.GetBuiltinType(btid));
}

sona::ref_ptr<AST::Expr const>
SemaPhase1::ActOnCharLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::CharLiteralExpr const> literalExpr) {
  return new AST::CharLiteralExpr(
             literalExpr->GetValue(),
             m_ASTContext.GetBuiltinType(
               AST::BuiltinType::BuiltinTypeId::BTI_Char));
}

sona::ref_ptr<AST::Expr const>
SemaPhase1::ActOnStringLiteralExpr(
    std::shared_ptr<Scope>,
    sona::ref_ptr<Syntax::StringLiteralExpr const> literalExpr) {
  return new AST::StringLiteralExpr(
               literalExpr->GetValue(),
               m_ASTContext.CreatePointerType(
                 m_ASTContext.GetBuiltinType(
                   AST::BuiltinType::BuiltinTypeId::BTI_Char)));
}

} // namespace Sema
} // namespace ckx
