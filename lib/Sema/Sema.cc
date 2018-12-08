#include "Sema/Sema.h"

#include "Syntax/CST.h"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Expr.hpp"
#include "AST/Type.hpp"

using namespace ckx;
using namespace ckx::Sema;
using namespace sona;

ref_ptr<AST::Type const>
SemaClass::ResolveType(ref_ptr<Syntax::Type const> type) {
  switch (type->GetNodeKind()) {
#define CST_TYPE(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return Resolve##name(static_cast<Syntax::name const&>(type.get()));
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
    return nullptr;
  }
}

owner<AST::Decl>
SemaClass::ActOnDecl(ref_ptr<Syntax::Decl const> decl) {
  switch (decl->GetNodeKind()) {
#define CST_DECL(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return ActOn##name(static_cast<Syntax::name const&>(decl.get()));
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
    return nullptr;
  }
}

/*

owner<AST::Stmt>
SemaClass::ActOnStmt(ref_ptr<Syntax::Stmt const> stmt) {
  switch (stmt->GetNodeKind()) {
#define CST_STMT(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return ActOn##name(static_cast<Syntax::name const&>(stmt.get()));
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
    return nullptr;
  }
}

owner<AST::Expr>
SemaClass::ActOnExpr(ref_ptr<Syntax::Expr const> expr) {
  switch (expr->GetNodeKind()) {
#define CST_EXPR(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return ActOn##name(static_cast<Syntax::name const&>(expr.get()));
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
    return nullptr;
  }
}

*/

ref_ptr<AST::Type const>
SemaClass::ResolveBasicType(ref_ptr<Syntax::BasicType const> type) {
}

ref_ptr<AST::Type const>
SemaClass::ResolveUserDefinedType(ref_ptr<Syntax::UserDefinedType const> type) {
  (void)type;
  sona_unreachable1("not implemented");
  return nullptr;
}

ref_ptr<AST::Type const>
SemaClass::ResolveTemplatedType(ref_ptr<Syntax::TemplatedType const> type) {
  (void)type;
  sona_unreachable1("not implemented");
  return nullptr;
}

ref_ptr<AST::Type const>
SemaClass::ResolveComposedType(ref_ptr<Syntax::ComposedType const> type) {
  (void)type;
  sona_unreachable1("not implemented");
  return nullptr;
}
