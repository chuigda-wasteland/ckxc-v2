#include "Sema/SemaPhase0.h"

#include "Syntax/CST.h"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Decl.hpp"
#include "AST/Type.hpp"

#include <functional>
#include <numeric>

#include <sona/linq.hpp>

using namespace ckx;
using namespace sona;

namespace ckx {
namespace Sema {

SemaPhase0::SemaPhase0(
    AST::ASTContext &astContext,
    std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
    std::vector<std::shared_ptr<Scope>> &scopeChains,
    Diag::DiagnosticEngine& diag)
  : SemaCommon(astContext, declContexts, scopeChains, diag) {}

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

void SemaPhase0::PostSubstituteDepends() {
  for (auto &ivar : m_IncompleteVars) {
    std::shared_ptr<Scope> inScope = ivar.second.GetEnclosingScope();
    for (auto &dep : ivar.second.GetDependencies()) {
      if (dep.IsDependByname()) {
        sona::ref_ptr<AST::Type const> type =
            LookupType(inScope, dep.GetIdUnsafe(), true);
        sona::ref_ptr<AST::Decl const> decl =
            AST::GetDeclOfUserDefinedType(type);
         dep.ReplaceNameWithDecl(decl);
      }
    }
  }

  for (auto &iusing : m_IncompleteUsings) {
    std::shared_ptr<Scope> inScope = iusing.second.GetEnclosingScope();
    for (auto &dep : iusing.second.GetDependencies()) {
      if (dep.IsDependByname()) {
        sona::ref_ptr<AST::Type const> type =
            inScope->LookupType(dep.GetIdUnsafe().GetIdentifier());
        sona::ref_ptr<AST::Decl const> decl =
            AST::GetDeclOfUserDefinedType(type);
         dep.ReplaceNameWithDecl(decl);
      }
    }
  }

  for (auto &idata : m_IncompleteEnumClassInterns) {
    std::shared_ptr<Scope> inScope = idata.second.GetEnclosingScope();
    for (auto &dep : idata.second.GetDependencies()) {
      if (dep.IsDependByname()) {
        sona::ref_ptr<AST::Type const> type =
            inScope->LookupType(dep.GetIdUnsafe().GetIdentifier());
        sona::ref_ptr<AST::Decl const> decl =
            AST::GetDeclOfUserDefinedType(type);
         dep.ReplaceNameWithDecl(decl);
      }
    }
  }
}

/// This algorithm directly comes from wikipedia:
///   https://en.wikipedia.org/wiki/Topological_sorting
std::vector<sona::ref_ptr<IncompleteDecl>>
SemaPhase0::FindTranslationOrder() {
  /// @todo add diagnostics info
  std::vector<sona::ref_ptr<IncompleteDecl>> transOrder;
  std::unordered_set<sona::ref_ptr<IncompleteDecl>> permanents;
  std::unordered_set<sona::ref_ptr<IncompleteDecl>> temporaries;

  std::function<bool(sona::ref_ptr<IncompleteDecl>)> VisitIncompleteDecl =
  [&, this](sona::ref_ptr<IncompleteDecl> decl) -> bool {
    if (permanents.find(decl) != permanents.cend()) {
      return true;
    }
    if (temporaries.find(decl) != temporaries.cend()) {
      return false;
    }

    temporaries.insert(decl);
    for (sona::ref_ptr<IncompleteDecl> dependingIncompleteDecl :
         sona::linq::from_container(decl->GetDependencies())
           .transform([](Sema::Dependency const& dep)
                      { return dep.GetDeclUnsafe(); })
           .transform([this](sona::ref_ptr<AST::Decl const> depDecl)
                      { return SearchInUnfinished(depDecl); })) {
      if (dependingIncompleteDecl == nullptr) {
        continue;
      }
      if (!VisitIncompleteDecl(dependingIncompleteDecl)) {
        return false;
      }
    }
    temporaries.erase(temporaries.find(decl));
    permanents.insert(decl);
    transOrder.push_back(decl);
    return true;
  };

  auto r1 = sona::linq::from_container(m_IncompleteVars).
            transform([](auto& p) -> sona::ref_ptr<IncompleteDecl>
                      { return static_cast<IncompleteDecl*>(&(p.second)); });
  auto r2 = sona::linq::from_container(m_IncompleteTags).
            transform([](auto& p) -> sona::ref_ptr<IncompleteDecl>
                      { return static_cast<IncompleteDecl*>(&(p.second)); });
  auto r3 = sona::linq::from_container(m_IncompleteUsings).
            transform([](auto& p) -> sona::ref_ptr<IncompleteDecl>
                      { return static_cast<IncompleteDecl*>(&(p.second)); });
  auto r4 = sona::linq::from_container(m_IncompleteEnumClassInterns).
            transform([](auto& p) -> sona::ref_ptr<IncompleteDecl>
                      { return static_cast<IncompleteDecl*>(&(p.second)); });
  for (sona::ref_ptr<IncompleteDecl> incomplete :
       r1.concat_with(r2).concat_with(r3).concat_with(r4)) {
    if (!VisitIncompleteDecl(incomplete)) {
      return transOrder;
    }
  }
  return transOrder;
}

std::vector<IncompleteFuncDecl> &SemaPhase0::GetIncompleteFuncs() {
  return m_IncompleteFuncs;
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
  return SemaCommon::ResolveBasicTypeImpl(bty);
}

sona::either<sona::ref_ptr<AST::Type const>, std::vector<Dependency>>
SemaPhase0::
ResolveUserDefinedType(std::shared_ptr<Scope> scope,
                       sona::ref_ptr<Syntax::UserDefinedType const> uty) {
  sona::ref_ptr<AST::Type const> lookupResult =
      LookupType(scope, uty->GetName(), false);
  if (lookupResult != nullptr) {
    if (!CheckTypeComplete(lookupResult)) {
      std::vector<Dependency> dependencies;
      dependencies.emplace_back(
        AST::GetDeclOfUserDefinedType(lookupResult), true);
      return std::move(dependencies);
    }

    return lookupResult;
  }

  std::vector<Dependency> dependencies;
  dependencies.emplace_back(uty->GetName().ExplicitlyClone(), true);

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
    sona::owner<AST::Decl> varDecl =
        new AST::VarDecl(GetCurrentDeclContext(), typeResult.as_t1(),
                         AST::DeclSpec::DS_None /** @todo  */,
                         decl->GetName());
    GetCurrentScope()->AddVarDecl(varDecl.borrow()
                                  .cast_unsafe<AST::VarDecl>());

    return std::make_pair(std::move(varDecl), true);
  }

  sona::owner<AST::Decl> incomplete =
      new AST::VarDecl(GetCurrentDeclContext(), nullptr,
                       AST::DeclSpec::DS_None /*TODO*/, decl->GetName());
  scope->AddVarDecl(incomplete.borrow()
                    .cast_unsafe<AST::VarDecl>());
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
  PushScope(Scope::SF_Class);

  for (sona::ref_ptr<Syntax::Decl const> subDecl : decl->GetSubDecls()) {
    auto p = ActOnDecl(GetCurrentScope(), subDecl);
    if (!p.second) {
      collectedDependencies.emplace_back(p.first.borrow(), true);
    }
    GetCurrentDeclContext()->AddDecl(std::move(p.first));
  }

  PopScope();
  PopDeclContext();

  GetCurrentScope()->AddType(
      decl->GetClassName(),
      m_ASTContext.AddUserDefinedType(
          new AST::ClassType(classDecl.borrow())));

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
  PushScope(Scope::SF_ADT);

  for (sona::ref_ptr<Syntax::ADTDecl::DataConstructor const> constructor
       : decl->GetConstructors()) {
    auto result = ActOnADTConstructor(GetCurrentScope(), constructor);
    if (!result.second) {
      collectedDependencies.emplace_back(result.first.borrow(), true);
    }
    GetCurrentDeclContext()->AddDecl(std::move(result.first));
  }

  PopScope();
  PopDeclContext();

  GetCurrentScope()->AddType(
      enumClassDecl.borrow()->GetName(),
      m_ASTContext.AddUserDefinedType(
          new AST::EnumClassType(enumClassDecl.borrow())));

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
            ret0.borrow(), dc, std::move(typeResult.as_t2()), scope));
  }

  return std::make_pair(std::move(ret0), typeResult.contains_t1());
}

sona::ref_ptr<IncompleteDecl>
SemaPhase0::SearchInUnfinished(sona::ref_ptr<const AST::Decl> decl) {
  switch (decl->GetDeclKind()) {
  case AST::Decl::DK_Var: {
    auto it = m_IncompleteVars.find(decl.cast_unsafe<AST::VarDecl const>());
    if (it != m_IncompleteVars.cend()) {
      return it->second;
    }
    return nullptr;
  }
  case AST::Decl::DK_Class:
  case AST::Decl::DK_Enum: {
    auto it = m_IncompleteTags.find(decl);
    if (it !=  m_IncompleteTags.cend()) {
      return it->second;
    }
    return nullptr;
  }
  case AST::Decl::DK_EnumClassIntern: {
    auto it = m_IncompleteEnumClassInterns.find(
                decl.cast_unsafe<AST::EnumClassInternDecl const>());
    if (it != m_IncompleteEnumClassInterns.cend()) {
      return it->second;
    }
    return nullptr;
  }
  default: {
    sona_unreachable();
  }
  }
  return nullptr;
}

bool SemaPhase0::CheckTypeComplete(sona::ref_ptr<const AST::Type> type) {
  switch (type->GetTypeId()) {
  case AST::Type::TypeId::TI_Builtin:
  case AST::Type::TypeId::TI_Pointer:
  case AST::Type::TypeId::TI_Ref:
    return true;

  case AST::Type::TypeId::TI_Array:
    return CheckTypeComplete(type.cast_unsafe<AST::ArrayType const>()
                                 ->GetBase());
  case AST::Type::TypeId::TI_Tuple: {
    sona::ref_ptr<AST::TupleType const> tupleType =
        type.cast_unsafe<AST::TupleType const>();
    return std::accumulate(tupleType->GetTupleElemTypes().begin(),
                           tupleType->GetTupleElemTypes().end(), true,
                           [this](bool cur, sona::ref_ptr<AST::Type const> ty) {
                             return cur && CheckTypeComplete(ty);
                           });
  }

  case AST::Type::TypeId::TI_UserDefined: {
    return CheckUserDefinedTypeComplete(
             type.cast_unsafe<AST::UserDefinedType const>());
  }

  default:
    sona_unreachable1("not implemented");
  }
  return false;
}

bool SemaPhase0::CheckUserDefinedTypeComplete(
    sona::ref_ptr<const AST::UserDefinedType> type) {
  sona::ref_ptr<AST::TypeDecl const> correspondingDecl = type->GetTypeDecl();
  if (type->GetUserDefinedTypeId()
      == AST::UserDefinedType::UDTypeId::UTI_Using) {
    auto it = m_IncompleteUsings.find(
                correspondingDecl.cast_unsafe<AST::UsingDecl const>());
    return it == m_IncompleteUsings.end();
  }
  else {
    auto it = m_IncompleteTags.find(
                correspondingDecl.cast_unsafe<AST::Decl const>());
    return it == m_IncompleteTags.end();
  }
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
  sona::ref_ptr<AST::UsingDecl> usingDecl =
      ret0.borrow().cast_unsafe<AST::UsingDecl>();
  GetCurrentScope()->AddType(
        usingDecl->GetName(),
        m_ASTContext.AddUserDefinedType(new AST::UsingType(usingDecl)));
  if (typeResult.contains_t2()) {
    m_IncompleteUsings.emplace(
          ret0.borrow().cast_unsafe<AST::UsingDecl>(),
          Sema::IncompleteUsingDecl(
            ret0.borrow().cast_unsafe<AST::UsingDecl>(),
            decl, std::move(typeResult.as_t2()), scope));
  }

  return std::make_pair(std::move(ret0), typeResult.contains_t1());
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnFuncDecl(std::shared_ptr<Scope>,
                          sona::ref_ptr<Syntax::FuncDecl const> decl) {
  m_IncompleteFuncs.emplace_back(decl, GetCurrentScope(),
                                 GetCurrentDeclContext());
  return std::make_pair(nullptr, false);
}

std::pair<sona::owner<AST::Decl>, bool>
SemaPhase0::ActOnEnumDecl(std::shared_ptr<Scope>,
                          sona::ref_ptr<Syntax::EnumDecl const> decl) {
  sona::owner<AST::EnumDecl> enumDecl =
      new AST::EnumDecl(GetCurrentDeclContext(), decl->GetName());
  PushDeclContext(enumDecl.borrow().cast_unsafe<AST::DeclContext>());
  PushScope(Scope::SF_Enum);

  std::int64_t value = 0;
  for (Syntax::EnumDecl::Enumerator const& e : decl->GetEnumerators()) {
    value = e.HasValue() ? e.GetValueUnsafe() : value;
    GetCurrentDeclContext()->AddDecl(
          new AST::EnumeratorDecl(GetCurrentDeclContext(), e.GetName(), value));
    value++;
  }

  PopScope();
  PopDeclContext();

  GetCurrentScope()->AddType(
        decl->GetName(),
        m_ASTContext.AddUserDefinedType(
          new AST::EnumType(enumDecl.borrow())));

  return std::make_pair(enumDecl.cast_unsafe<AST::Decl>(), true);
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
