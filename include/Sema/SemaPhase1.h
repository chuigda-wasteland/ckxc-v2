#ifndef SEMA_PHASE1_H
#define SEMA_PHASE1_H

#include "Sema/SemaCommon.h"
#include "sona/either.hpp"

namespace ckx {
namespace Sema {

class SemaPhase1 : public SemaCommon {
public:
  SemaPhase1(AST::ASTContext &astContext,
             std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
             std::vector<std::shared_ptr<Scope>> &scopeChains,
             Diag::DiagnosticEngine &diag);

  void PostTranslateIncompletes(
      std::vector<sona::ref_ptr<Sema::IncompleteDecl>> incompletes);

  void TranslateFunctions(std::vector<IncompleteFuncDecl> &funcs);

protected:
  sona::ref_ptr<AST::Type const>
  ResolveType(std::shared_ptr<Scope> scope,
              sona::ref_ptr<Syntax::Type const> type);

  void PostTranslateIncompleteVar(sona::ref_ptr<Sema::IncompleteVarDecl> iVar);
  void PostTranslateIncompleteTag(sona::ref_ptr<Sema::IncompleteTagDecl> iTag);
  void PostTranslateIncompleteADTConstructor(sona::ref_ptr<Sema::IncompleteEnumClassInternDecl> iAdtC);
  void PostTranslateIncompleteUsing(
      sona::ref_ptr<Sema::IncompleteUsingDecl> iusing);

#define CST_TYPE(name) \
  sona::ref_ptr<AST::Type const> \
  Resolve##name(std::shared_ptr<Scope> scope, \
                sona::ref_ptr<Syntax::name const> type);

#include "Syntax/CSTNodeDefs.def"

};

} // namespace Sema
} // namespace ckx

#endif // SEMA_PHASE1_H
