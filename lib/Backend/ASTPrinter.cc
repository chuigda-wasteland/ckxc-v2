#include "Backend/ASTPrinter.h"

namespace ckx {
namespace Backend {

sona::owner<DeclResult>
ASTPrinter::VisitTransUnit(
    sona::ref_ptr<AST::TransUnitDecl const> transUnitDecl) {
  (void)transUnitDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitLabelDecl(sona::ref_ptr<AST::LabelDecl const> labelDecl) {
  (void)labelDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitClassDecl(sona::ref_ptr<AST::ClassDecl const> classDecl) {
  (void)classDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitEnumDecl(sona::ref_ptr<AST::EnumDecl const> enumDecl) {
  (void)enumDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitEnumeratorDecl(
    sona::ref_ptr<AST::EnumeratorDecl const> enumeratorDecl) {
  (void)enumeratorDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitEnumClassInternDecl(
    sona::ref_ptr<AST::EnumClassInternDecl const> eciDecl) {
  (void)eciDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitEnumClassDecl(
    sona::ref_ptr<AST::EnumClassDecl const> enumClassDecl) {
  (void)enumClassDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitUsingDecl(sona::ref_ptr<AST::UsingDecl const> usingDecl) {
  (void)usingDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitFuncDecl(sona::ref_ptr<AST::FuncDecl const> funcDecl) {
  (void)funcDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitVarDecl(sona::ref_ptr<AST::VarDecl const> varDecl) {
  (void)varDecl;
  return CreateDeclResult(VoidType());
}

} // namespace Backend
} // namespace ckx
