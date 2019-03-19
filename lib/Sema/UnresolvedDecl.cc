#include "Sema/UnresolvedDecl.hpp"
#include "Sema/Scope.h"
#include "AST/Type.hpp"

namespace ckx {
namespace Sema {

void IncompleteDecl::AddNameDepend(Syntax::Identifier&& id, bool isStrong) {
  m_Dependencies.emplace_back(std::move(id), isStrong);
}

void IncompleteDecl::AddValueDepend(sona::ref_ptr<AST::Decl> decl,
                                    bool isStrong) {
  m_Dependencies.emplace_back(decl, isStrong);
}

} // namespace Sema
} // namespace ckx
