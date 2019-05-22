#include "Backend/ASTPrinter.h"

#include <iostream>

namespace ckx {
namespace Backend {

sona::owner<ActionResult>
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

sona::owner<ActionResult>
ASTPrinter::VisitLabelDecl(sona::ref_ptr<AST::LabelDecl const> labelDecl) {
  Indent();
  std::cerr << "Label declaraion " << labelDecl->GetLabelString().get()
            << " @" << (void*)this << std::endl;
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
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

sona::owner<ActionResult>
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

sona::owner<ActionResult>
ASTPrinter::VisitEnumeratorDecl(
    sona::ref_ptr<AST::EnumeratorDecl const> enumeratorDecl) {
  Indent();
  std::cerr << "Enumerator declaration "
            << enumeratorDecl->GetEnumeratorName().get()
            << " @" << (void*)this << std::endl;
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitEnumClassInternDecl(
    sona::ref_ptr<AST::EnumClassInternDecl const> eciDecl) {
  Indent();
  std::cerr << "ADT constructor declaration "
            << eciDecl->GetConstructorName().get()
            << " of type @" << (void*)(eciDecl->GetType().operator->())
            << " @" << (void*)this << std::endl;
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
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

sona::owner<ActionResult>
ASTPrinter::VisitUsingDecl(sona::ref_ptr<AST::UsingDecl const> usingDecl) {
  (void)usingDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitFuncDecl(sona::ref_ptr<AST::FuncDecl const> funcDecl) {
  (void)funcDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitVarDecl(sona::ref_ptr<AST::VarDecl const> varDecl) {
  (void)varDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitBuiltinType(
    sona::ref_ptr<const AST::BuiltinType> builtinType) {

}

sona::owner<ActionResult>
ASTPrinter::VisitTupleType(
    sona::ref_ptr<const AST::TupleType> tupleType) {

}

sona::owner<ActionResult>
ASTPrinter::VisitArrayType(
    sona::ref_ptr<const AST::ArrayType> arrayType) {

}

sona::owner<ActionResult>
ASTPrinter::VisitPointerType(
    sona::ref_ptr<const AST::PointerType> ptrType) {

}

sona::owner<ActionResult>
ASTPrinter::VisitLValueRefType(
    sona::ref_ptr<const AST::LValueRefType> lvRefType) {

}

sona::owner<ActionResult>
ASTPrinter::VisitRValueRefType(
    sona::ref_ptr<const AST::RValueRefType> rvRefType) {

}

sona::owner<ActionResult>
ASTPrinter::VisitFunctionType(
    sona::ref_ptr<const AST::FunctionType> funcType) {

}

sona::owner<ActionResult>
ASTPrinter::VisitClassType(
    sona::ref_ptr<const AST::ClassType> classType) {

}

sona::owner<ActionResult>
ASTPrinter::VisitEnumType(
    sona::ref_ptr<const AST::EnumType> enumType) {

}

sona::owner<ActionResult>
ASTPrinter::VisitEnumClassType(
    sona::ref_ptr<const AST::EnumClassType> enumClassType) {

}

sona::owner<ActionResult>
ASTPrinter::VisitUsingType(sona::ref_ptr<const AST::UsingType> usingType) {

}

void ASTPrinter::Indent() const {
  for (std::size_t i = 0; i < m_NestedLevel * m_IndentSize; i++) {
    std::cerr << ' ';
  }
}

} // namespace Backend
} // namespace ckx
