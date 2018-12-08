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
  AST::BuiltinType::BuiltinTypeId btid;
  switch (type->GetTypeKind()) {
  case Syntax::BasicType::TypeKind::TK_Int8:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_i8; break;
  case Syntax::BasicType::TypeKind::TK_Int16:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_i16; break;
  case Syntax::BasicType::TypeKind::TK_Int32:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_i32; break;
  case Syntax::BasicType::TypeKind::TK_Int64:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_i64; break;
  case Syntax::BasicType::TypeKind::TK_UInt8:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_u8; break;
  case Syntax::BasicType::TypeKind::TK_UInt16:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_u16; break;
  case Syntax::BasicType::TypeKind::TK_UInt32:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_u32; break;
  case Syntax::BasicType::TypeKind::TK_UInt64:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_u64; break;
  case Syntax::BasicType::TypeKind::TK_Float:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_r32; break;
  case Syntax::BasicType::TypeKind::TK_Double:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_r64; break;
  case Syntax::BasicType::TypeKind::TK_Quad:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_r128; break;
  case Syntax::BasicType::TypeKind::TK_Bool:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_bool; break;
  case Syntax::BasicType::TypeKind::TK_Void:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_void; break;
  default:
    sona_unreachable1("unhandled case");
  }

  return m_ASTContext.GetBuiltinType(btid);
}

ref_ptr<AST::Type const>
SemaClass::ResolveUserDefinedType(ref_ptr<Syntax::UserDefinedType const> type) {
  return m_ScopeChains.back().LookupType(type->GetName());
}

ref_ptr<AST::Type const>
SemaClass::ResolveTemplatedType(ref_ptr<Syntax::TemplatedType const> type) {
  (void)type;
  sona_unreachable1("not implemented");
  return nullptr;
}

ref_ptr<AST::Type const>
SemaClass::ResolveComposedType(ref_ptr<Syntax::ComposedType const> type) {
  ref_ptr<Syntax::Type const> rootType = type->GetRootType();
  ref_ptr<AST::Type const> resolvedRootType = ResolveType(rootType);

  if (resolvedRootType == nullptr) {
    return nullptr;
  }

  for (Syntax::ComposedType::TypeSpecifier tySpec : type->GetTypeSpecifiers()) {
    /// @todo add proper diagnostics
    switch (tySpec) {
    case Syntax::ComposedType::TypeSpecifier::CTS_Const:
    case Syntax::ComposedType::TypeSpecifier::CTS_Volatile:
      sona_unreachable1("not implemented");
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_Pointer:
      resolvedRootType =
        m_ASTContext.CreatePointerType(resolvedRootType).get();
    case Syntax::ComposedType::TypeSpecifier::CTS_Ref:
      resolvedRootType =
        m_ASTContext.CreateLValueRefType(resolvedRootType).get();
    case Syntax::ComposedType::TypeSpecifier::CTS_RvRef:
      resolvedRootType =
        m_ASTContext.CreateRValueRefType(resolvedRootType).get();
    }
  }

  return resolvedRootType;
}
