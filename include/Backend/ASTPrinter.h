#ifndef ASTPRINTER_H
#define ASTPRINTER_H

#include "Backend/ASTVisitor.h"

#include "AST/Decl.hpp"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Type.hpp"

namespace ckx {
namespace Backend {

class ASTPrinter final : public DeclVisitor {
public:
  ASTPrinter(std::size_t indentSize = 2) : m_IndentSize(indentSize) {}

  sona::owner<ActionResult>
  VisitTransUnit(
      sona::ref_ptr<AST::TransUnitDecl const> transUnitDecl) override;

  sona::owner<ActionResult>
  VisitLabelDecl(sona::ref_ptr<AST::LabelDecl const> labelDecl) override;

  sona::owner<ActionResult>
  VisitClassDecl(sona::ref_ptr<AST::ClassDecl const> classDecl) override;

  sona::owner<ActionResult>
  VisitEnumDecl(sona::ref_ptr<AST::EnumDecl const> enumDecl) override;

  sona::owner<ActionResult>
  VisitEnumeratorDecl(
      sona::ref_ptr<AST::EnumeratorDecl const> enumeratorDecl) override;

  sona::owner<ActionResult>
  VisitEnumClassInternDecl(
      sona::ref_ptr<AST::EnumClassInternDecl const> eciDecl) override;

  sona::owner<ActionResult>
  VisitEnumClassDecl(
      sona::ref_ptr<AST::EnumClassDecl const> enumClassDecl) override;

  sona::owner<ActionResult>
  VisitUsingDecl(sona::ref_ptr<AST::UsingDecl const> usingDecl) override;

  sona::owner<ActionResult>
  VisitFuncDecl(sona::ref_ptr<AST::FuncDecl const> funcDecl) override;

  sona::owner<ActionResult>
  VisitVarDecl(sona::ref_ptr<AST::VarDecl const> varDecl) override;

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
