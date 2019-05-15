#include "Sema/SemaPhase1.h"

namespace ckx {
namespace Sema {

SemaPhase1::SemaPhase1(
    AST::ASTContext &astContext,
    std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
    std::vector<std::shared_ptr<Scope> > &scopeChains,
    Diag::DiagnosticEngine &diag)
  : SemaCommon(astContext, declContexts, scopeChains, diag) {}

sona::ref_ptr<const AST::Type>
SemaPhase1::ResolveType(std::shared_ptr<Scope> scope,
                        sona::ref_ptr<const Syntax::Type> type) {
  switch (type->GetNodeKind()) {
#define CST_TYPE(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return Resolve##name(scope, type.cast_unsafe<Syntax::name const>());
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
  }
  return sona::ref_ptr<AST::Type const>(nullptr);
}

sona::ref_ptr<AST::Type const>
SemaPhase1::ResolveBasicType(std::shared_ptr<Scope>,
                             sona::ref_ptr<Syntax::BasicType const> bty) {
  return SemaCommon::ResolveBasicTypeImpl(bty);
}

sona::ref_ptr<AST::Type const>
SemaPhase1::
ResolveUserDefinedType(std::shared_ptr<Scope> scope,
                       sona::ref_ptr<Syntax::UserDefinedType const> uty) {
  sona::ref_ptr<AST::Type const> lookupResult =
      LookupType(scope, uty->GetName(), false);
  sona_assert(lookupResult != nullptr);
  return lookupResult;
}

sona::ref_ptr<AST::Type const>
SemaPhase1::
ResolveTemplatedType(std::shared_ptr<Scope>,
                     sona::ref_ptr<Syntax::TemplatedType const>) {
  sona_unreachable1("not implemented");
  return sona::ref_ptr<AST::Type const>(nullptr);
}

sona::ref_ptr<AST::Type const>
SemaPhase1::
ResolveComposedType(std::shared_ptr<Scope> scope,
                    sona::ref_ptr<Syntax::ComposedType const> cty) {
  sona::ref_ptr<AST::Type const> ret
      = ResolveType(scope, cty->GetRootType());

  for (Syntax::ComposedType::TypeSpecifier ts : cty->GetTypeSpecifiers()) {
    switch (ts) {
    case Syntax::ComposedType::TypeSpecifier::CTS_Pointer:
      ret = m_ASTContext.CreatePointerType(ret)
                        .cast_unsafe<AST::Type const>();
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_Ref:
      ret = m_ASTContext.CreateLValueRefType(ret)
                        .cast_unsafe<AST::Type const>();
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_RvRef:
      ret = m_ASTContext.CreateRValueRefType(ret)
                        .cast_unsafe<AST::Type const>();
      break;
    default:
      sona_unreachable1("not implemented");
    }
  }
  return ret;
}

} // namespace Sema
} // namespace ckx
