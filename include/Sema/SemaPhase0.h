#ifndef SEMA_H
#define SEMA_H

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
#include "sona/either.hpp"

namespace ckx {
namespace Sema {

class SemaPhase0 final {
public:
  SemaPhase0(Diag::DiagnosticEngine &diag);

  sona::owner<AST::TransUnitDecl>
  ActOnTransUnit(sona::ref_ptr<Syntax::TransUnit> transUnit);

private:
  void PushScope(Scope::ScopeFlags flags = Scope::SF_None);
  void PopScope();
  std::shared_ptr<Scope> const& CurrentScope() const noexcept;

  sona::either<sona::ref_ptr<AST::Type const>,  // finally resolved type
               std::vector<Dependency>>         // dependencies
  ResolveType(std::shared_ptr<Scope> scope,
              sona::ref_ptr<Syntax::Type const> type);

  std::pair<sona::owner<AST::Decl>, bool>
  ActOnDecl(std::shared_ptr<Scope> scope,
            sona::ref_ptr<Syntax::Decl const> decl);

#define CST_TYPE(name) \
  sona::either<sona::ref_ptr<AST::Type const>, \
               std::vector<Dependency>> \
  Resolve##name(std::shared_ptr<Scope> scope, \
                sona::ref_ptr<Syntax::name const> type);

#define CST_DECL(name) \
  std::pair<sona::owner<AST::Decl>, bool> \
  ActOn##name(std::shared_ptr<Scope> scope, \
              sona::ref_ptr<Syntax::name const> decl);

  std::pair<sona::owner<AST::Decl>, bool>
  ActOnADTConstructor(std::shared_ptr<Scope> scope,
                      sona::ref_ptr<Syntax::ADTDecl::DataConstructor const> dc);

#include "Syntax/CSTNodeDefs.def"

private:
  void PushDeclContext(sona::ref_ptr<AST::DeclContext> context);
  void PopDeclContext();
  sona::ref_ptr<AST::DeclContext> GetCurrentDeclContext();
  std::shared_ptr<Scope> GetCurrentScope();

  Diag::DiagnosticEngine &m_Diag;

  AST::ASTContext m_ASTContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> m_DeclContexts;
  std::vector<std::shared_ptr<Scope>> m_ScopeChains;
  std::vector<Syntax::Export> m_Exports;

  std::vector<Sema::IncompleteVarDecl> m_IncompleteVars;
  std::vector<Sema::IncompleteTagDecl> m_IncompleteTags;
  std::vector<Sema::IncompleteEnumClassInternDecl> m_IncompleteEnumClassInterns;
};

} // namespace Sema
} // namespace ckx

#endif // CSTANNOTATOR_H
