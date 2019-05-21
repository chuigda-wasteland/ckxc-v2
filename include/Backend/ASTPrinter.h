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
  sona::owner<DeclResult>
  VisitTransUnit(
      sona::ref_ptr<AST::TransUnitDecl const> transUnitDecl) override;

  sona::owner<DeclResult>
  VisitLabelDecl(sona::ref_ptr<AST::LabelDecl const> labelDecl) override;

  sona::owner<DeclResult>
  VisitClassDecl(sona::ref_ptr<AST::ClassDecl const> classDecl) override;

  sona::owner<DeclResult>
  VisitEnumDecl(sona::ref_ptr<AST::EnumDecl const> enumDecl) override;

  sona::owner<DeclResult>
  VisitEnumeratorDecl(
      sona::ref_ptr<AST::EnumeratorDecl const> enumeratorDecl) override;

  sona::owner<DeclResult>
  VisitEnumClassInternDecl(
      sona::ref_ptr<AST::EnumClassInternDecl const> eciDecl) override;

  sona::owner<DeclResult>
  VisitEnumClassDecl(
      sona::ref_ptr<AST::EnumClassDecl const> enumClassDecl) override;

  sona::owner<DeclResult>
  VisitUsingDecl(sona::ref_ptr<AST::UsingDecl const> usingDecl) override;

  sona::owner<DeclResult>
  VisitFuncDecl(sona::ref_ptr<AST::FuncDecl const> funcDecl) override;

  sona::owner<DeclResult>
  VisitVarDecl(sona::ref_ptr<AST::VarDecl const> varDecl) override;
};

}
} // namespace ckx

#endif // ASTPRINTER_H
