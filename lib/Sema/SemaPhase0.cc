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

sona::either<sona::ref_ptr<AST::Type const>, std::vector<Dependency>>
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
  return sona::ref_ptr<AST::Type const>(nullptr);
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
  return nullptr;
}

sona::either<sona::ref_ptr<AST::Type const>, std::vector<Dependency>>
SemaPhase0::ResolveBasicType(std::shared_ptr<Scope>,
                             sona::ref_ptr<Syntax::BasicType const> bty) {
  AST::BuiltinType::BuiltinTypeId bid;
  /// @todo consider use tablegen to generate this, or unify the two
  /// "type kinds" enumeration
  switch (bty->GetTypeKind()) {
  case Syntax::BasicType::TypeKind::TK_Int8:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_i8; break;
  case Syntax::BasicType::TypeKind::TK_Int16:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_i16; break;
  case Syntax::BasicType::TypeKind::TK_Int32:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_i32; break;
  case Syntax::BasicType::TypeKind::TK_Int64:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_i64; break;
  case Syntax::BasicType::TypeKind::TK_UInt8:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_u8; break;
  case Syntax::BasicType::TypeKind::TK_UInt16:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_u16; break;
  case Syntax::BasicType::TypeKind::TK_UInt32:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_u32; break;
  case Syntax::BasicType::TypeKind::TK_UInt64:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_u64; break;
  case Syntax::BasicType::TypeKind::TK_Float:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_r32; break;
  case Syntax::BasicType::TypeKind::TK_Double:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_r64; break;
  /// @todo quad type is platform dependent, consider use a PlatformConfig
  /// class to control this
  case Syntax::BasicType::TypeKind::TK_Quad:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_r128; break;
  case Syntax::BasicType::TypeKind::TK_Bool:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_bool; break;
  case Syntax::BasicType::TypeKind::TK_Void:
    bid = AST::BuiltinType::BuiltinTypeId::BTI_void; break;
  }

  return m_ASTContext.GetBuiltinType(bid);
}

sona::either<sona::ref_ptr<AST::Type const>, std::vector<Dependency>>
SemaPhase0::
ResolveUserDefinedType(std::shared_ptr<Scope> scope,
                       sona::ref_ptr<Syntax::UserDefinedType const> uty) {
  sona::ref_ptr<AST::Type const> lookupResult =
      scope->LookupType(uty->GetName());
  if (lookupResult != nullptr) {
    return lookupResult;
  }

  std::vector<Dependency> dependencies;
  dependencies.emplace_back(
    Syntax::Identifier(uty->GetName(), uty->GetSourceRange()));
  return dependencies;
}

sona::either<sona::ref_ptr<AST::Type const>, std::vector<Dependency>>
SemaPhase0::
ResolveTemplatedType(std::shared_ptr<Scope>,
                     sona::ref_ptr<Syntax::TemplatedType const>) {
  sona_unreachable1("not implemented");
  return sona::ref_ptr<AST::Type const>(nullptr);
}

sona::either<sona::ref_ptr<AST::Type const>, std::vector<Dependency>>
SemaPhase0::
ResolveComposedType(std::shared_ptr<Scope>,
                    sona::ref_ptr<Syntax::ComposedType const>) {
  /// @todo differ from strong dependency and weak dependency, and we may get on
  /**
  auto rootTypeResult = ResolveType(scope, cty->GetRootType());
  if (rootTypeResult.contains_t1()
      || std::any_of(
           cty->GetTypeSpecifiers().begin(),
           cty->GetTypeSpecifiers().end(),
           [](Syntax::ComposedType::TypeSpecifier ts) {
             return ts == Syntax::ComposedType::TypeSpecifier::CTS_Pointer;
           }
      )) {
    sona::ref_ptr<AST::Type const> rootType = rootTypeResult.as_t1();
    for (Syntax::ComposedType::TypeSpecifier ts : cty->GetTypeSpecifiers()) {
      switch (ts) {
      case Syntax::ComposedType::TypeSpecifier::CTS_Const:
      case Syntax::ComposedType::TypeSpecifier::CTS_Volatile:
        sona_unreachable1("not implemented");
        break;
      /// @todo need more checks
      case Syntax::ComposedType::TypeSpecifier::CTS_Pointer:
        rootType = new AST::PointerType(rootType);
        break;
      case Syntax::ComposedType::TypeSpecifier::CTS_Ref:
        rootType = new AST::LValueRefType(rootType);
        break;
      case Syntax::ComposedType::TypeSpecifier::CTS_RvRef:
        rootType = new AST::RValueRefType(rootType);
        break;
      }
    }
    return rootType;
  }
  */
  sona_unreachable1("not implemented");
  return sona::ref_ptr<AST::Type const>(nullptr);
}

} // namespace Sema
} // namespace ckx
