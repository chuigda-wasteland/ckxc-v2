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

class SemaPhase0 {
public:
  SemaPhase0(Diag::DiagnosticEngine &diag);

  sona::owner<AST::TransUnitDecl>
  ActOnTransUnit(sona::ref_ptr<Syntax::TransUnit> transUnit);

  void PostSubstituteDepends();

  std::vector<sona::ref_ptr<Sema::IncompleteDecl>> FindTranslationOrder();

private:
  void PushScope(Scope::ScopeFlags flags = Scope::SF_None);
  void PopScope();

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

#include "Syntax/CSTNodeDefs.def"

  std::pair<sona::owner<AST::Decl>, bool>
  ActOnADTConstructor(std::shared_ptr<Scope> scope,
                      sona::ref_ptr<Syntax::ADTDecl::DataConstructor const> dc);

  sona::ref_ptr<IncompleteDecl>
  SearchInUnfinished(sona::ref_ptr<AST::Decl const> decl);

  sona::ref_ptr<AST::Type const> LookupType(std::shared_ptr<Scope> scope,
                                      Syntax::Identifier const& identifier,
                                      bool shouldDiag);

  sona::ref_ptr<AST::DeclContext>
  ChooseDeclContext(std::shared_ptr<Scope> scope,
                    std::vector<sona::string_ref> const& nns, bool shouldDiag);

protected:
  /// @note the following interfaces are opened for testing
  void PushDeclContext(sona::ref_ptr<AST::DeclContext> context);
  void PopDeclContext();
  sona::ref_ptr<AST::DeclContext> GetCurrentDeclContext();
  std::shared_ptr<Scope> const& GetCurrentScope() const noexcept;

  bool CheckTypeComplete(sona::ref_ptr<AST::Type const> type);
  bool CheckUserDefinedTypeComplete(
      sona::ref_ptr<AST::UserDefinedType const> type);

  Diag::DiagnosticEngine &m_Diag;

  AST::ASTContext m_ASTContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> m_DeclContexts;
  std::vector<std::shared_ptr<Scope>> m_ScopeChains;
  std::vector<Syntax::Export> m_Exports;

  std::unordered_map<sona::ref_ptr<AST::VarDecl const>,
                     Sema::IncompleteVarDecl>
    m_IncompleteVars;
  std::unordered_map<sona::ref_ptr<AST::Decl const>,
                     Sema::IncompleteTagDecl>
    m_IncompleteTags;
  std::unordered_map<sona::ref_ptr<AST::UsingDecl const>,
                     Sema::IncompleteUsingDecl>
    m_IncompleteUsings;
  std::unordered_map<sona::ref_ptr<AST::EnumClassInternDecl const>,
                     Sema::IncompleteEnumClassInternDecl>
    m_IncompleteEnumClassInterns;
  std::vector<Sema::IncompleteFuncDecl> m_IncompleteFuncs;
};

} // namespace Sema
} // namespace ckx

#endif // CSTANNOTATOR_H
