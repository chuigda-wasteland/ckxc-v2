#include "Sema/SemaPhase0.h"

#include "Syntax/CST.h"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Expr.hpp"
#include "AST/Type.hpp"

using namespace ckx;
using namespace sona;

namespace ckx {
namespace Sema {

SemaPhase0::SemaPhase0(Diag::DiagnosticEngine& diag) : m_Diag(diag) {}

sona::owner<AST::TransUnitDecl>
SemaPhase0::ActOnTransUnit(sona::ref_ptr<Syntax::TransUnit> transUnit) {
  sona::owner<AST::TransUnitDecl> transUnitDecl = new AST::TransUnitDecl();
  for (ref_ptr<Syntax::Decl const> decl : transUnit->GetDecls()) {
    transUnitDecl.borrow()->AddDecl(ActOnDecl(CurrentScope(), decl));
  }
  return transUnitDecl;
}

sona::either<sona::ref_ptr<AST::Type const>, std::vector<Syntax::Identifier>>
SemaPhase0::ResolveType(std::shared_ptr<Scope> scope,
                        sona::ref_ptr<Syntax::Type const> type) {
  switch (type->GetNodeKind()) {
#define CST_TYPE(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return Resolve##name(scope, type.cast_unsafe<Syntax::name const>());
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
  }
  return sona::either<sona::ref_ptr<AST::Type const>,
                      std::vector<Syntax::Identifier>>(nullptr);
}

sona::owner<AST::Decl>
SemaPhase0::ActOnDecl(std::shared_ptr<Scope> scope,
                      sona::ref_ptr<const Syntax::Decl> decl) {
  switch (decl->GetNodeKind()) {
#define CST_DECL(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return ActOn##name(scope, decl.cast_unsafe<Syntax::name const>());
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
  }
  // for silencing compiler warnings
  return nullptr;
}

} // namespace Sema
} // namespace ckx
