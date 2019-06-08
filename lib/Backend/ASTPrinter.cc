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
ASTPrinter::VisitValueCtor(
    sona::ref_ptr<AST::ValueCtorDecl const> valueCtorDecl) {
  Indent();
  std::cerr << "ADT constructor declaration "
            << valueCtorDecl->GetConstructorName().get()
            << " of type ";
  valueCtorDecl->GetType().GetUnqualTy()->Accept(this);
  std::cerr << " @" << (void*)this << std::endl;
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitADT(
    sona::ref_ptr<AST::ADTDecl const> adtDecl) {
  Indent();
  std::cerr << "ADT declaraion " << adtDecl->GetName().get()
            << " @" << (void*)this << std::endl;
  EnterScope();
  for (auto decl : adtDecl->GetDecls()) {
    decl->Accept(this);
  }
  ExitScope();
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitUsingDecl(sona::ref_ptr<AST::UsingDecl const> usingDecl) {
  Indent();
  std::cerr << "Using declaration " << usingDecl->GetName().get()
            << " aliasing to ";
  usingDecl->GetTypeForDecl()->Accept(this);
  std::cerr << " @ " << (void*)this << std::endl;
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitFuncDecl(sona::ref_ptr<AST::FuncDecl const> funcDecl) {
  (void)funcDecl;
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitVarDecl(sona::ref_ptr<AST::VarDecl const> varDecl) {
  Indent();
  std::cerr << "Variable declaration " << varDecl->GetVarName().get()
            << " of type ";
  varDecl->GetType().GetUnqualTy()->Accept(this);
  std::cerr << " @ " << (void*)this << std::endl;
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitBuiltinType(
    sona::ref_ptr<const AST::BuiltinType> builtinType) {
  std::cerr << builtinType->GetTypeName();
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitTupleType(
    sona::ref_ptr<const AST::TupleType> tupleType) {
  std::cerr << "T(";
  for (auto& tupleElem : tupleType->GetTupleElemTypes()) {
    tupleElem.GetUnqualTy()->Accept(this);
    std::cerr << ", ";
  }
  std::cerr << ")";
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitArrayType(
    sona::ref_ptr<const AST::ArrayType> arrayType) {
  std::cerr << "ArrayOf(";
  arrayType->GetBase().GetUnqualTy()->Accept(this);
  std::cerr << ")";
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitPointerType(
    sona::ref_ptr<const AST::PointerType> ptrType) {
  std::cerr << "PointerTo(";
  ptrType->GetPointee().GetUnqualTy()->Accept(this);
  std::cerr << ")";
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitLValueRefType(
    sona::ref_ptr<const AST::LValueRefType> lvRefType) {
  std::cerr << "LValueRefTo(";
  lvRefType->GetReferencedType().GetUnqualTy()->Accept(this);
  std::cerr << ")";
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitRValueRefType(
    sona::ref_ptr<const AST::RValueRefType> rvRefType) {
  std::cerr << "RValueRefTo(";
  rvRefType->GetReferencedType().GetUnqualTy()->Accept(this);
  std::cerr << ")";
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitFunctionType(
    sona::ref_ptr<const AST::FunctionType> funcType) {
  std::cerr << "Function(";
  for (const auto& paramType : funcType->GetParamTypes()) {
    paramType.GetUnqualTy()->Accept(this);
  }
  std::cerr << ")->";
  funcType->GetReturnType().GetUnqualTy()->Accept(this);
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitClassType(
    sona::ref_ptr<const AST::ClassType> classType) {
  std::cerr << "class " << classType->GetClassDecl()->GetName().get()
            << " @" << classType->GetTypeDecl().operator->();
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitEnumType(
    sona::ref_ptr<const AST::EnumType> enumType) {
  std::cerr << "enum " << enumType->GetEnumDecl()->GetName().get()
            << " @" << enumType->GetTypeDecl().operator->();
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitADTType(
    sona::ref_ptr<const AST::ADTType> adtType) {
  std::cerr << "ADT " << adtType->GetADTDecl()->GetName().get()
            << " @" << adtType->GetTypeDecl().operator->();
  return CreateDeclResult(VoidType());
}

sona::owner<ActionResult>
ASTPrinter::VisitUsingType(sona::ref_ptr<const AST::UsingType> usingType) {
  std::cerr << "Alias " << usingType->GetTypeName().get() << " to (";
  usingType->GetUsingDecl()->GetAliasee().GetUnqualTy()->Accept(this);
  std::cerr << ")";
  return CreateDeclResult(VoidType());
}

void ASTPrinter::Indent() const {
  for (std::size_t i = 0; i < m_NestedLevel * m_IndentSize; i++) {
    std::cerr << ' ';
  }
}

} // namespace Backend
} // namespace ckx
