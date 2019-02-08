#include "Sema/Scope.h"

using namespace ckx;
using namespace ckx::Sema;

Scope::Scope(std::shared_ptr<Scope> parentScope, Scope::ScopeFlags scopeFlags)
  : m_ParentScope(parentScope), m_EnclosingFunctionScope(nullptr),
    m_EnclosingLoopScope(nullptr), m_ScopeFlags(scopeFlags) {
  for (sona::ref_ptr<Scope> scope = this;
       scope != nullptr;
       scope = scope->GetParentScope().get()) {
    if (scope->HasFlags(SF_InLoop)) {
      m_EnclosingLoopScope = scope;
      break;
    }
  }

  for (sona::ref_ptr<Scope> scope = this;
       scope != nullptr;
       scope = scope->GetParentScope().get()) {
    if (scope->HasFlags(SF_Function)) {
      m_EnclosingFunctionScope = scope;
      break;
    }
  }
}

void Scope::AddVarDecl(sona::ref_ptr<const AST::VarDecl> varDecl) {
  m_Variables.emplace(varDecl->GetVarName(), varDecl);
}

void Scope::AddType(sona::string_ref const& typeName,
                    sona::ref_ptr<const AST::Type> type) {
  m_Types.emplace(typeName, type);
}

void Scope::AddFunction(sona::ref_ptr<const AST::FuncDecl> funcDecl) {
  m_Functions.emplace(funcDecl->GetName(), funcDecl);
}

sona::ref_ptr<AST::VarDecl const>
Scope::LookupVarDecl(const sona::string_ref &name) const noexcept {
  for (sona::ref_ptr<Scope const> s = this; s != nullptr;
       s = s->GetParentScope()) {
    sona::ref_ptr<AST::VarDecl const> localResult =
        s->LookupVarDeclLocally(name);
    if (localResult != nullptr) {
      return localResult;
    }
  }
  return nullptr;
}

sona::ref_ptr<AST::Type const>
Scope::LookupType(const sona::string_ref &name) const noexcept {
  for (sona::ref_ptr<Scope const> s = this; s != nullptr;
       s = s->GetParentScope()) {
    sona::ref_ptr<AST::Type const> localResult =
        s->LookupTypeLocally(name);
    if (localResult != nullptr) {
      return localResult;
    }
  }
  return nullptr;
}

sona::ref_ptr<const AST::Type>
Scope::LookupTypeLocally(const sona::string_ref& name) const noexcept {
  auto it = m_Types.find(name);
  if (it != m_Types.cend()) {
    return it->second;
  }
  return nullptr;
}

sona::ref_ptr<const AST::VarDecl>
Scope::LookupVarDeclLocally(const sona::string_ref& name) const noexcept {
  auto it = m_Variables.find(name);
  if (it != m_Variables.cend()) {
    return it->second;
  }
  return nullptr;
}

sona::iterator_range<Scope::FunctionSet::const_iterator>
Scope::GetAllFuncsLocal(const sona::string_ref &name) const noexcept {
  auto it = m_Functions.find(name);
  if (it == m_Functions.cend()) {
    return sona::iterator_range<FunctionSet::const_iterator>(it, it);
  }

  auto it2 = it;
  ++it2;
  for (; it2 != m_Functions.cend() && it2->first == name; ++it2);
  return sona::iterator_range<FunctionSet::const_iterator>(it, it2);
}

sona::iterator_range<Scope::FunctionSet::const_iterator>
Scope::GetAllFuncs(const sona::string_ref &name) const noexcept {
  auto localResult = GetAllFuncsLocal(name);
  if (localResult.size() == 0 && m_ParentScope != nullptr) {
    return m_ParentScope->GetAllFuncs(name);
  }
  return localResult;
}
