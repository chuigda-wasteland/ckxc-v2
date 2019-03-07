#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "Syntax/CST.h"
#include "AST/DeclFwd.hpp"
#include "sona/either.hpp"

namespace ckx {
namespace Sema {

class Dependency {
public:
  Dependency(Syntax::Identifier const& id)
    : m_Dependency(id) {}

  Dependency(sona::ref_ptr<AST::Decl> decl)
    : m_Dependency(decl) {}

  bool IsDependByname() const noexcept {
    return m_Dependency.contains_t1();
  }

private:
  sona::either<Syntax::Identifier, sona::ref_ptr<AST::Decl>> m_Dependency;
};

} // namespace Sema
} // namespace ckx

#endif // DEPENDENCY_H