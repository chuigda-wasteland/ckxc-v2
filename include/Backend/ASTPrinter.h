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


  sona::owner<ActionResult>
  VisitBuiltinType(sona::ref_ptr<AST::BuiltinType const> builtinType) override;

  sona::owner<ActionResult>
  VisitTupleType(sona::ref_ptr<AST::TupleType const> tupleType) override;

  sona::owner<ActionResult>
  VisitArrayType(sona::ref_ptr<AST::ArrayType const> arrayType) override;

  sona::owner<ActionResult>
  VisitPointerType(sona::ref_ptr<AST::PointerType const> ptrType) override;

  sona::owner<ActionResult>
  VisitLValueRefType(
      sona::ref_ptr<AST::LValueRefType const> lvRefType) override;

  sona::owner<ActionResult>
  VisitRValueRefType(
      sona::ref_ptr<AST::RValueRefType const> rvRefType) override;

  sona::owner<ActionResult>
  VisitFunctionType(sona::ref_ptr<AST::FunctionType const> funcType) override;

  sona::owner<ActionResult>
  VisitClassType(sona::ref_ptr<AST::ClassType const> classType) override;

  sona::owner<ActionResult>
  VisitEnumType(sona::ref_ptr<AST::EnumType const> enumType) override;

  sona::owner<ActionResult>
  VisitEnumClassType(
      sona::ref_ptr<AST::EnumClassType const> enumClassType) override;

  sona::owner<ActionResult>
  VisitUsingType(sona::ref_ptr<AST::UsingType const> usingType) override;

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
