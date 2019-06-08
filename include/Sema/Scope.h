#ifndef SCOPE_H
#define SCOPE_H

#include "AST/Decl.h"
#include "AST/Type.h"
#include "Sema/UnresolvedDecl.h"

#include <map>
#include <cstdint>

namespace ckx {
namespace Sema {

class Scope {
public:
  using FunctionSet =
  std::map<sona::strhdl_t, sona::ref_ptr<AST::FuncDecl const>>;

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

  void AddType(const sona::strhdl_t& typeName,
               sona::ref_ptr<AST::Type const> type);

  void AddFunction(sona::ref_ptr<AST::FuncDecl const> funcDecl);

  sona::ref_ptr<AST::Type const>
  LookupType(sona::strhdl_t const& name) const noexcept;

  sona::ref_ptr<AST::VarDecl const>
  LookupVarDecl(sona::strhdl_t const& name) const noexcept;

  sona::ref_ptr<AST::Type const>
  LookupTypeLocally(sona::strhdl_t const& name) const noexcept;

  sona::ref_ptr<AST::VarDecl const>
  LookupVarDeclLocally(sona::strhdl_t const& name) const noexcept;

  sona::iterator_range<FunctionSet::const_iterator>
  GetAllFuncsLocal(sona::strhdl_t const& name) const noexcept;

  sona::iterator_range<FunctionSet::const_iterator>
  GetAllFuncs(sona::strhdl_t const& name) const noexcept;

  void SetDeclContext(sona::ref_ptr<AST::DeclContext> declContext) noexcept {
    m_UnderlyingDeclContext = declContext;
  }

  sona::ref_ptr<AST::DeclContext> GetDeclContext() noexcept {
    return m_UnderlyingDeclContext;
  }

  sona::ref_ptr<AST::DeclContext const> GetDeclContext() const noexcept {
    return m_UnderlyingDeclContext;
  }

  /// @note only used for replacing unresolved var declaration
  /// into abstract declaraion
  void ReplaceVarDecl(sona::strhdl_t const& denotingName,
                      sona::ref_ptr<AST::VarDecl const> varDecl);

private:
  std::shared_ptr<Scope> m_ParentScope;
  sona::ref_ptr<Scope> m_EnclosingFunctionScope;
  sona::ref_ptr<Scope> m_EnclosingLoopScope;
  ScopeFlags m_ScopeFlags;

  std::unordered_map<sona::strhdl_t, sona::ref_ptr<AST::Decl const>>
  m_Tags;
  std::unordered_map<sona::strhdl_t, sona::ref_ptr<AST::VarDecl const>>
  m_Variables;
  std::unordered_map<sona::strhdl_t, sona::ref_ptr<AST::Type const>>
  m_Types;

  FunctionSet m_Functions;

  /// @todo I'm also not sure if this will be used, letus keep it for sometime
  sona::ref_ptr<AST::DeclContext> m_UnderlyingDeclContext = nullptr;
};

} // namespace Sema
} // namespace ckx

#endif // SCOPE_H
