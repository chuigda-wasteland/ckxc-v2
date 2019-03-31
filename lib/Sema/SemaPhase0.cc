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
  PushDeclContext(transUnitDecl.borrow().cast_unsafe<AST::DeclContext>());
  PushScope();
  for (ref_ptr<Syntax::Decl const> decl : transUnit->GetDecls()) {
    transUnitDecl.borrow()->AddDecl(ActOnDecl(GetCurrentScope(), decl).first);
  }
  return transUnitDecl;
}

void SemaPhase0::PushScope(Scope::ScopeFlags flags) {
  m_ScopeChains.emplace_back(
      new Scope(m_ScopeChains.empty() ? nullptr : m_ScopeChains.back(), flags));
}

void SemaPhase0::PopScope() {
  m_ScopeChains.pop_back();
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

std::pair<sona::owner<AST::Decl>, bool>
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
  return std::make_pair(nullptr, false);
}

sona::either<sona::ref_ptr<AST::Type const>, std::vector<Dependency>>
SemaPhase0::ResolveBasicType(std::shared_ptr<Scope> /* unused */,
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
    if (!CheckTypeComplete(lookupResult)) {
      std::vector<Dependency> dependencies;
      dependencies.emplace_back(
        AST::GetDeclOfUserDefinedType(lookupResult), true);
    }

    return lookupResult;
  }

  std::vector<Dependency> dependencies;
  dependencies.emplace_back(
    Syntax::Identifier(uty->GetName(), uty->GetSourceRange()), true);

  return sona::either<sona::ref_ptr<AST::Type const>,
                      std::vector<Dependency>>(std::move(dependencies));
}

sona::either<sona::ref_ptr<AST::Type const>, std::vector<Dependency>>
SemaPhase0::
ResolveTemplatedType(std::shared_ptr<Scope>,
                     sona::ref_ptr<Syntax::TemplatedType const>) {
  sona_unreachable1("not implemented");
  return sona::ref_ptr<AST::Type const>(nullptr);
}

static bool IsPtrOrRefType(sona::ref_ptr<Syntax::ComposedType const> cty) {
  return std::any_of(
        cty->GetTypeSpecifiers().begin(),
        cty->GetTypeSpecifiers().end(),
        [](Syntax::ComposedType::TypeSpecifier ts) {
          return ts == Syntax::ComposedType::TypeSpecifier::CTS_Pointer
                 || ts == Syntax::ComposedType::TypeSpecifier::CTS_Ref
                 || ts == Syntax::ComposedType::TypeSpecifier::CTS_RvRef;
  });
}

sona::either<sona::ref_ptr<AST::Type const>, std::vector<Dependency>>
SemaPhase0::
ResolveComposedType(std::shared_ptr<Scope> scope,
                    sona::ref_ptr<Syntax::ComposedType const> cty) {
  auto rootTypeResult = ResolveType(scope, cty->GetRootType());
  if (rootTypeResult.contains_t1()) {
    auto ret = rootTypeResult.as_t1();
    for (Syntax::ComposedType::TypeSpecifier ts : cty->GetTypeSpecifiers()) {
      switch (ts) {
      case Syntax::ComposedType::TypeSpecifier::CTS_Pointer:
        ret = new AST::PointerType(ret); break;
      case Syntax::ComposedType::TypeSpecifier::CTS_Ref:
        ret = new AST::LValueRefType(ret); break;
      case Syntax::ComposedType::TypeSpecifier::CTS_RvRef:
        ret = new AST::RValueRefType(ret); break;
      default:
        sona_unreachable1("not implemented");
      }
    }
    return ret;
  }

  auto dependencies = std::move(rootTypeResult.as_t2());
  if (IsPtrOrRefType(cty)) {
    for (Dependency &dependency : dependencies) {
      dependency.SetStrong(false);
    }
  }

  return sona::either<sona::ref_ptr<AST::Type const>,
                      std::vector<Dependency>>(std::move(dependencies));
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnVarDecl(std::shared_ptr<Scope> scope,
                         sona::ref_ptr<Syntax::VarDecl const> decl) {
  auto typeResult = ResolveType(scope, decl->GetType());
  if (typeResult.contains_t1()) {
    return std::make_pair(
        sona::owner<AST::Decl>(
            new AST::VarDecl(GetCurrentDeclContext(), typeResult.as_t1(),
                             AST::DeclSpec::DS_None /** @todo  */,
                              decl->GetName())),
            true);
  }

  sona::owner<AST::Decl> incomplete =
      new AST::VarDecl(GetCurrentDeclContext(), nullptr,
                       AST::DeclSpec::DS_None /*TODO*/, decl->GetName());
  m_IncompleteVars.emplace(
        incomplete.borrow().cast_unsafe<AST::VarDecl>(),
        Sema::IncompleteVarDecl(incomplete.borrow().cast_unsafe<AST::VarDecl>(),
                                decl, GetCurrentDeclContext(),
                                std::move(typeResult.as_t2()), scope));
  return std::make_pair(std::move(incomplete), false);
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnClassDecl(std::shared_ptr<Scope> scope,
                           sona::ref_ptr<Syntax::ClassDecl const> decl) {
  std::vector<Dependency> collectedDependencies;
  sona::owner<AST::ClassDecl> classDecl =
      new AST::ClassDecl(GetCurrentDeclContext(), decl->GetClassName());
  PushDeclContext(classDecl.borrow().cast_unsafe<AST::DeclContext>());

  for (sona::ref_ptr<Syntax::Decl const> subDecl : decl->GetSubDecls()) {
    auto p = ActOnDecl(scope, subDecl);
    if (!p.second) {
      collectedDependencies.emplace_back(p.first.borrow(), true);
    }
    GetCurrentDeclContext()->AddDecl(std::move(p.first));
  }

  PopDeclContext();

  if (!collectedDependencies.empty()) {
    m_IncompleteTags.emplace(
          classDecl.borrow().cast_unsafe<AST::Decl>(),
          IncompleteTagDecl(classDecl.borrow().cast_unsafe<AST::Decl>(),
                            std::move(collectedDependencies), scope));
  }

  return std::make_pair(classDecl.cast_unsafe<AST::Decl>(),
                        !collectedDependencies.empty());
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnADTDecl(std::shared_ptr<Scope> scope,
                         sona::ref_ptr<Syntax::ADTDecl const> decl) {
  std::vector<Dependency> collectedDependencies;
  sona::owner<AST::EnumClassDecl> enumClassDecl =
    new AST::EnumClassDecl(GetCurrentDeclContext(), decl->GetName());
  PushDeclContext(enumClassDecl.borrow().cast_unsafe<AST::DeclContext>());

  for (sona::ref_ptr<Syntax::ADTDecl::DataConstructor const> constructor
       : decl->GetConstructors()) {
    auto result = ActOnADTConstructor(scope, constructor);
    if (!result.second) {
      collectedDependencies.emplace_back(result.first.borrow(), true);
    }
    GetCurrentDeclContext()->AddDecl(std::move(result.first));
  }

  PopDeclContext();

  if (!collectedDependencies.empty()) {
    m_IncompleteTags.emplace(
          enumClassDecl.borrow().cast_unsafe<AST::Decl>(),
          IncompleteTagDecl(
            enumClassDecl.borrow().cast_unsafe<AST::Decl>(),
            std::move(collectedDependencies), scope));
  }

  return std::make_pair(enumClassDecl.cast_unsafe<AST::Decl>(),
                        !collectedDependencies.empty());
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnADTConstructor(
    std::shared_ptr<Scope> scope,
    sona::ref_ptr<const Syntax::ADTDecl::DataConstructor> dc) {
  auto typeResult = ResolveType(scope, dc->GetUnderlyingType());
  sona::owner<AST::Decl> ret0 =
      typeResult.contains_t1() ?
        new AST::EnumClassInternDecl(GetCurrentDeclContext(),
                                     dc->GetName(), typeResult.as_t1())
      : new AST::EnumClassInternDecl(GetCurrentDeclContext(),
                                     dc->GetName(), nullptr);
  if (typeResult.contains_t2()) {
    m_IncompleteEnumClassInterns.emplace(
          ret0.borrow().cast_unsafe<AST::EnumClassInternDecl>(),
          Sema::IncompleteEnumClassInternDecl(
            ret0.borrow(), std::move(typeResult.as_t2()), scope));
  }

  return std::make_pair(std::move(ret0), typeResult.contains_t1());
}

void SemaPhase0::PushDeclContext(sona::ref_ptr<AST::DeclContext> context) {
  m_DeclContexts.push_back(context);
}

void SemaPhase0::PopDeclContext() {
  m_DeclContexts.pop_back();
}

sona::ref_ptr<AST::DeclContext> SemaPhase0::GetCurrentDeclContext() {
  return m_DeclContexts.back();
}

std::shared_ptr<Scope> const& SemaPhase0::GetCurrentScope() const noexcept {
  return m_ScopeChains.back();
}

bool SemaPhase0::CheckTypeComplete(sona::ref_ptr<const AST::Type> type) {
  switch (type->GetTypeId()) {
  case AST::Type::TypeId::TI_Tag: {
    return CheckTagTypeComplete(
             type.cast_unsafe<AST::TagType const>());
  }

  case AST::Type::TypeId::TI_Using: {
    sona::ref_ptr<AST::UsingType const> usingType =
        type.cast_unsafe<AST::UsingType const>();
    auto it = m_IncompleteUsings.find(usingType->GetUsingDecl());
    return it != m_IncompleteUsings.end();
  }

  default: {
    sona_unreachable1("checking completion of non- user defined types");
  }
  }
  return false;
}

bool SemaPhase0::CheckTagTypeComplete(
    sona::ref_ptr<const AST::TagType> type) {
  sona::ref_ptr<AST::Decl const> correspondingDecl = nullptr;
  switch (type->GetTagTypeId()) {
  case AST::TagType::UDTypeId::TTI_Class:
    correspondingDecl = type.cast_unsafe<AST::ClassType const>()->GetDecl()
                            .cast_unsafe<AST::Decl const>();
  case AST::TagType::UDTypeId::TTI_Enum:
    correspondingDecl = type.cast_unsafe<AST::EnumType const>()->GetDecl()
                            .cast_unsafe<AST::Decl const>();
  case AST::TagType::UDTypeId::TTI_EnumClass:
    correspondingDecl = type.cast_unsafe<AST::EnumClassType const>()->GetDecl()
                            .cast_unsafe<AST::Decl const>();
  }

  auto it = m_IncompleteTags.find(correspondingDecl);
  return it != m_IncompleteTags.end();
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnUsingDecl(std::shared_ptr<Scope> scope,
                           sona::ref_ptr<Syntax::UsingDecl const> decl) {
  auto typeResult = ResolveType(scope, decl->GetAliasee());
  sona::owner<AST::Decl> ret0 =
      typeResult.contains_t1() ?
        new AST::UsingDecl(GetCurrentDeclContext(),
                           decl->GetName(), typeResult.as_t1())
      : new AST::UsingDecl(GetCurrentDeclContext(),
                           decl->GetName(), nullptr);
  if (typeResult.contains_t2()) {
    m_IncompleteUsings.emplace(
          ret0.borrow().cast_unsafe<AST::UsingDecl>(),
          Sema::IncompleteUsingDecl(
            ret0.borrow().cast_unsafe<AST::UsingDecl>(),
            std::move(typeResult.as_t2()), scope));
  }

  return std::make_pair(std::move(ret0), typeResult.contains_t1());
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnFuncDecl(std::shared_ptr<Scope> scope,
                          sona::ref_ptr<Syntax::FuncDecl const> decl) {
  (void)scope;
  (void)decl;
  sona_unreachable1("not implemented");
  return std::make_pair(nullptr, false);
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnEnumDecl(std::shared_ptr<Scope> scope,
                          sona::ref_ptr<Syntax::EnumDecl const> decl) {
  (void)scope;
  (void)decl;
  sona_unreachable1("not implemented");
  return std::make_pair(nullptr, false);
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnForwardDecl(std::shared_ptr<Scope> scope,
                             sona::ref_ptr<Syntax::ForwardDecl const> decl) {
  (void)scope;
  (void)decl;
  sona_unreachable1("not implemented");
  return std::make_pair(nullptr, false);
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnTemplatedDecl(
    std::shared_ptr<Scope> scope,
    sona::ref_ptr<Syntax::TemplatedDecl const> decl) {
  (void)scope;
  (void)decl;
  sona_unreachable1("not implemented");
  return std::make_pair(nullptr, false);
}

} // namespace Sema
} // namespace ckx
