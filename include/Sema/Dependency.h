#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "Syntax/CST.h"
#include "AST/DeclFwd.hpp"
#include "sona/either.hpp"

namespace ckx {
namespace Sema {

class Dependency {
public:
  Dependency(Syntax::Identifier &&id, bool isStrong = false)
    : m_Dependency(std::move(id)), m_IsStrong(isStrong) {}

  Dependency(sona::ref_ptr<AST::Decl const> decl, bool isStrong = false)
    : m_Dependency(decl), m_IsStrong(isStrong) {}

  bool IsDependByname() const noexcept {
    return m_Dependency.contains_t1();
  }

  void ReplaceNameWithDecl(sona::ref_ptr<AST::Decl const> decl) noexcept {
    m_Dependency.set(decl);
  }

  Syntax::Identifier const& GetIdUnsafe() const noexcept {
    return m_Dependency.as_t1();
  }

  sona::ref_ptr<AST::Decl const> GetDeclUnsafe() const noexcept {
    return m_Dependency.as_t2();
  }

  bool IsStrong() const noexcept {
    return m_IsStrong;
  }

  void SetStrong(bool strong = true) noexcept {
    m_IsStrong = strong;
  }

private:
  sona::either<Syntax::Identifier, sona::ref_ptr<AST::Decl const>> m_Dependency;
  bool m_IsStrong;
};

} // namespace Sema
} // namespace ckx

#endif // DEPENDENCY_H
