#ifndef SCOPE_H
#define SCOPE_H

#include "AST/Decl.hpp"
#include "AST/Type.hpp"

#include <map>
#include <cstdint>

namespace ckx {
namespace Sema {

class Scope {
public:
  using FunctionSet =
  std::map<sona::string_ref, sona::ref_ptr<AST::FuncDecl const>>;

  enum ScopeFlags {
    SF_None          = 0x0000,
    SF_Function      = 0x0001,
    SF_FunctionParam = 0x0002,
    SF_InLoop        = 0x0004,
    SF_InMatch       = 0x0008,
    SF_Block         = 0x0010,
    SF_Class         = 0x0020,
    SF_Enum          = 0x0040,
    SF_ADT           = 0x0080,
    SF_Unsafe        = 0x0100
  };

  Scope(std::shared_ptr<Scope> parentScope = nullptr,
        ScopeFlags scopeFlags = SF_None);

  std::shared_ptr<Scope> GetParentScope() const noexcept {
    return m_ParentScope;
  }

  sona::ref_ptr<Scope> GetEnclosingFunctionScope() const noexcept {
    return m_EnclosingFunctionScope;
  }

  sona::ref_ptr<Scope> GetEnclosingLoopScope() const noexcept {
    return m_EnclosingLoopScope;
  }

  ScopeFlags GetScopeFlags() const noexcept {
    return m_ScopeFlags;
  }

  ScopeFlags GetFlags() const noexcept {
    return m_ScopeFlags;
  }

  bool HasFlags(ScopeFlags scopeFlags) noexcept {
    return GetFlags() & scopeFlags;
  }

  void AddVarDecl(sona::ref_ptr<AST::VarDecl const> varDecl);

  void AddType(const sona::string_ref& typeName,
               sona::ref_ptr<AST::Type const> type);

  void AddFunction(sona::ref_ptr<AST::FuncDecl const> funcDecl);

  sona::ref_ptr<AST::VarDecl const>
  LookupVarDecl(sona::string_ref const& name) const noexcept;

  sona::ref_ptr<AST::Type const>
  LookupType(sona::string_ref const& name) const noexcept;

  sona::ref_ptr<AST::Type const>
  LookupTypeLocally(sona::string_ref const& name) const noexcept;

  sona::ref_ptr<AST::VarDecl const>
  LookupVarDeclLocally(sona::string_ref const& name) const noexcept;

  sona::iterator_range<FunctionSet::const_iterator>
  GetAllFuncsLocal(sona::string_ref const& name) const noexcept;

  sona::iterator_range<FunctionSet::const_iterator>
  GetAllFuncs(sona::string_ref const& name) const noexcept;

private:
  std::shared_ptr<Scope> m_ParentScope;
  sona::ref_ptr<Scope> m_EnclosingFunctionScope;
  sona::ref_ptr<Scope> m_EnclosingLoopScope;
  ScopeFlags m_ScopeFlags;

  std::unordered_map<sona::string_ref, sona::ref_ptr<AST::VarDecl const>>
  m_Variables;
  std::unordered_map<sona::string_ref, sona::ref_ptr<AST::Type const>>
  m_Types;

  FunctionSet m_Functions;
};

} // namespace Sema
} // namespace ckx

#endif // SCOPE_H
