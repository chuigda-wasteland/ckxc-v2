#ifndef SEMACOMMON_H
#define SEMACOMMON_H

#include "Sema/Scope.h"
#include "Sema/UnresolvedDecl.h"
#include "Sema/Dependency.h"

#include "Basic/Diagnose.h"
#include "Syntax/Concrete.h"
#include "AST/DeclFwd.h"
#include "AST/ExprFwd.h"
#include "AST/StmtFwd.h"
#include "AST/TypeFwd.h"

#include "sona/pointer_plus.h"

namespace ckx {
namespace Sema {

class SemaCommon {
public:
  SemaCommon(AST::ASTContext &astContext,
             std::vector<sona::ref_ptr<AST::DeclContext>> &declContexts,
             Diag::DiagnosticEngine &diag)
    : m_ASTContext(astContext), m_DeclContexts(declContexts),
      m_CurrentScope(nullptr), m_GlobalScope(nullptr), m_Diag(diag) {}

protected:
  static AST::BuiltinType::BuiltinTypeId
  ClassifyBuiltinTypeId(std::int64_t i) noexcept;
  static AST::BuiltinType::BuiltinTypeId
  ClassifyBuiltinTypeId(std::uint64_t u) noexcept;
  static AST::BuiltinType::BuiltinTypeId
  ClassifyBuiltinTypeId(double f) noexcept;

protected:
  void PushDeclContext(sona::ref_ptr<AST::DeclContext> context);
  void PopDeclContext();
  sona::ref_ptr<AST::DeclContext> GetCurrentDeclContext();
  std::shared_ptr<Scope> const& GetCurrentScope() const noexcept;
  std::shared_ptr<Scope> const& GetGlobalScope() const noexcept;
  void PushScope(Scope::ScopeFlags flags = Scope::SF_None);
  void PopScope();

  AST::QualType
  ResolveBuiltinTypeImpl(sona::ref_ptr<Syntax::BuiltinType const> basicType);

  sona::ref_ptr<const AST::DeclContext>
  ChooseDeclContext(std::shared_ptr<Scope> scope,
                    const std::vector<sona::strhdl_t>& nns, bool shouldDiag,
                    const std::vector<SingleSourceRange>& nnsRanges);

  AST::QualType LookupType(std::shared_ptr<Scope> scope, Syntax::Identifier const& identifier,
             bool shouldDiag);

  AST::ASTContext &m_ASTContext;
  std::vector<sona::ref_ptr<AST::DeclContext>> &m_DeclContexts;
  std::shared_ptr<Scope> m_CurrentScope;
  std::shared_ptr<Scope> m_GlobalScope;
  Diag::DiagnosticEngine &m_Diag;
};

} // namespace Sema
} // namespace ckx

#endif // SEMACOMMON_H
