#ifndef SEMACOMMON_H
#define SEMACOMMON_H

#include "Sema/Scope.h"
#include "Sema/UnresolvedDecl.hpp"
#include "Sema/Dependency.h"

#include "Basic/Diagnose.h"
#include "Syntax/CST.h"
#include "AST/DeclFwd.hpp"
#include "AST/ExprFwd.hpp"
#include "AST/StmtFwd.hpp"
#include "AST/TypeFwd.hpp"

#include "sona/pointer_plus.hpp"

namespace ckx {
namespace Sema {

class SemaCommon {
public:
  SemaCommon(AST::ASTContext &astContext,
             std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
             std::vector<std::shared_ptr<Scope>> &scopeChains,
             Diag::DiagnosticEngine &diag)
    : m_ASTContext(astContext),
      m_DeclContexts(declContexts),
      m_ScopeChains(scopeChains),
      m_Diag(diag) {}

protected:
  void PushDeclContext(sona::ref_ptr<AST::DeclContext> context);
  void PopDeclContext();
  sona::ref_ptr<AST::DeclContext> GetCurrentDeclContext();
  std::shared_ptr<Scope> const& GetCurrentScope() const noexcept;
  std::shared_ptr<Scope> const& GetGlobalScope() const noexcept;

   sona::ref_ptr<AST::BuiltinType const>
   ActOnBasicTypeImpl(sona::ref_ptr<Syntax::BasicType const> basicType);

   sona::ref_ptr<const AST::DeclContext>
   ChooseDeclContext(std::shared_ptr<Scope> scope,
                     const std::vector<sona::string_ref>& nns,
                     bool shouldDiag,
                     const std::vector<SingleSourceRange>& nnsRanges);

   sona::ref_ptr<AST::Type const>
   LookupType(std::shared_ptr<Scope> scope,
              Syntax::Identifier const& identifier,
              bool shouldDiag);

  AST::ASTContext &m_ASTContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> &m_DeclContexts;
  std::vector<std::shared_ptr<Scope>> &m_ScopeChains;
  Diag::DiagnosticEngine &m_Diag;
};

} // namespace Sema
} // namespace ckx

#endif // SEMACOMMON_H