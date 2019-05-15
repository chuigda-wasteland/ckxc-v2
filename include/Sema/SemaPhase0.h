#ifndef SEMA_H
#define SEMA_H

#include "Sema/SemaCommon.h"
#include "sona/either.hpp"

namespace ckx {
namespace Sema {

class SemaPhase0 : public SemaCommon {
public:
  SemaPhase0(AST::ASTContext &astContext,
             std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
             std::vector<std::shared_ptr<Scope>> &scopeChains,
             Diag::DiagnosticEngine &diag);

  sona::owner<AST::TransUnitDecl>
  ActOnTransUnit(sona::ref_ptr<Syntax::TransUnit> transUnit);

  void PostSubstituteDepends();

  std::vector<sona::ref_ptr<Sema::IncompleteDecl>> FindTranslationOrder();

protected:
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

protected:
  bool CheckTypeComplete(sona::ref_ptr<AST::Type const> type);
  bool CheckUserDefinedTypeComplete(
      sona::ref_ptr<AST::UserDefinedType const> type);

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

#endif // SEMA_H
