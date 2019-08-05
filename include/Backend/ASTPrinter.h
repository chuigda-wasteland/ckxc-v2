#ifndef ASTPRINTER_H
#define ASTPRINTER_H

#include "Backend/ASTVisitor.h"

#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/Type.h"

namespace ckx {
namespace Backend {

class ASTPrinter final : public DeclVisitor, public TypeVisitor {
public:
  ASTPrinter(std::size_t indentSize = 2) : m_IndentSize(indentSize) {}

#define AST_DECL(name) \
  sona::owner<ActionResult> \
  Visit##name(sona::ref_ptr<AST::name const> expr) override;
#include "AST/Nodes.def"

#define AST_TYPE(name) \
  sona::owner<ActionResult> \
  Visit##name(sona::ref_ptr<AST::name const> decl) override;
#include "AST/Nodes.def"

private:
  void EnterScope() noexcept {
    ++m_NestedLevel;
  }

  void ExitScope() noexcept {
    --m_NestedLevel;
  }

  void Indent() const;

  std::size_t m_NestedLevel = 0;
  std::size_t m_IndentSize;
};

} // namespace Backend
} // namespace ckx

#endif // ASTPRINTER_H
