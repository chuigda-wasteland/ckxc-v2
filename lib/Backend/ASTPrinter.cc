#include "Backend/ASTPrinter.h"

#include <iostream>

namespace ckx {
namespace Backend {

sona::owner<DeclResult>
ASTPrinter::VisitTransUnit(
    sona::ref_ptr<AST::TransUnitDecl const> transUnitDecl) {
  Indent();
  std::cerr << "Translation unit declaraion @" << (void*)this << std::endl;
  EnterScope();
  for (auto decl : transUnitDecl->GetDecls()) {
    decl->Accept(this);
  }
  ExitScope();
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitLabelDecl(sona::ref_ptr<AST::LabelDecl const> labelDecl) {
  Indent();
  std::cerr << "Label declaraion " << labelDecl->GetLabelString().get()
            << " @" << (void*)this << std::endl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitClassDecl(sona::ref_ptr<AST::ClassDecl const> classDecl) {
  Indent();
  std::cerr << "Class declaraion " << classDecl->GetName().get()
            << " @" << (void*)this << std::endl;
  EnterScope();
  for (auto decl : classDecl->GetDecls()) {
    decl->Accept(this);
  }
  ExitScope();
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitEnumDecl(sona::ref_ptr<AST::EnumDecl const> enumDecl) {
  Indent();
  std::cerr << "Enum declaraion " << enumDecl->GetName().get()
            << " @" << (void*)this << std::endl;
  EnterScope();
  for (auto enumerator : enumDecl->GetDecls()) {
    enumerator->Accept(this);
  }
  ExitScope();
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitEnumeratorDecl(
    sona::ref_ptr<AST::EnumeratorDecl const> enumeratorDecl) {
  Indent();
  std::cerr << "Enumerator declaration "
            << enumeratorDecl->GetEnumeratorName().get()
            << " @" << (void*)this << std::endl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitEnumClassInternDecl(
    sona::ref_ptr<AST::EnumClassInternDecl const> eciDecl) {
  Indent();
  std::cerr << "ADT constructor declaration "
            << eciDecl->GetConstructorName().get()
            << " of type @" << (void*)(eciDecl->GetType().operator->())
            << " @" << (void*)this << std::endl;
  return CreateDeclResult(VoidType());
}

sona::owner<DeclResult>
ASTPrinter::VisitEnumClassDecl(
    sona::ref_ptr<AST::EnumClassDecl const> enumClassDecl) {
  Indent();
  std::cerr << "ADT declaraion " << enumClassDecl->GetName().get()
            << " @" << (void*)this << std::endl;
  EnterScope();
  for (auto decl : enumClassDecl->GetDecls()) {
    decl->Accept(this);
  }
  ExitScope();
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

void ASTPrinter::Indent() const {
  for (std::size_t i = 0; i < m_NestedLevel * m_IndentSize; i++) {
    std::cerr << ' ';
  }
}

} // namespace Backend
} // namespace ckx
