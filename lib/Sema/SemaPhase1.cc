#include "Sema/SemaPhase1.h"

namespace ckx {
namespace Sema {

SemaPhase1::SemaPhase1(
    AST::ASTContext &astContext,
    std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
    std::vector<std::shared_ptr<Scope> > &scopeChains,
    Diag::DiagnosticEngine &diag)
  : SemaCommon(astContext, declContexts, scopeChains, diag) {}

} // namespace Sema
} // namespace ckx
