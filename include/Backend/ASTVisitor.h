#ifndef ASTVISITOR_H
#define ASTVISITOR_H

#include "AST/DeclFwd.hpp"
#include "AST/StmtFwd.hpp"
#include "AST/ExprFwd.hpp"
#include "AST/TypeFwd.hpp"

#include "sona/pointer_plus.hpp"
#include "sona/util.hpp"

namespace ckx {
namespace Backend {

class ActionResult {
public:
  virtual ~ActionResult() = 0;
};

template <typename T>
class ActionResultImpl : public ActionResult {
public:
  ActionResultImpl(T&& value) : m_Value(std::forward<T>(value)) {}
  ActionResultImpl(ActionResultImpl const&) = delete;
  ActionResultImpl(ActionResultImpl&&) = delete;
  ActionResultImpl& operator=(ActionResultImpl const&) = delete;
  ActionResultImpl& operator=(ActionResultImpl &&) = delete;

  T const& GetValue() const noexcept { return m_Value; }

private:
  T m_Value;
};

class VoidType {};

template<>
class ActionResultImpl<VoidType> : public ActionResult {
public:
  ActionResultImpl(VoidType v) { (void)v; }
  ActionResultImpl(ActionResultImpl const&) = delete;
  ActionResultImpl(ActionResultImpl&&) = delete;
  ActionResultImpl& operator=(ActionResultImpl const&) = delete;
  ActionResultImpl& operator=(ActionResultImpl &&) = delete;

  void* GetValue() const noexcept { return nullptr; }
};

template <typename T> sona::owner<ActionResult> CreateDeclResult(T&& t) {
  return new ActionResultImpl<T>(std::forward<T>(t));
}

class DeclVisitor {
public:
  virtual sona::owner<ActionResult>
  VisitTransUnit(sona::ref_ptr<AST::TransUnitDecl const> transUnitDecl) = 0;

  virtual sona::owner<ActionResult>
  VisitLabelDecl(sona::ref_ptr<AST::LabelDecl const> labelDecl) = 0;

  virtual sona::owner<ActionResult>
  VisitClassDecl(sona::ref_ptr<AST::ClassDecl const> classDecl) = 0;

  virtual sona::owner<ActionResult>
  VisitEnumDecl(sona::ref_ptr<AST::EnumDecl const> enumDecl) = 0;

  virtual sona::owner<ActionResult>
  VisitEnumeratorDecl(
      sona::ref_ptr<AST::EnumeratorDecl const> enumeratorDecl) = 0;

  virtual sona::owner<ActionResult>
  VisitEnumClassInternDecl(
      sona::ref_ptr<AST::EnumClassInternDecl const> eciDecl) = 0;

  virtual sona::owner<ActionResult>
  VisitEnumClassDecl(sona::ref_ptr<AST::EnumClassDecl const> enumClassDecl) = 0;

  virtual sona::owner<ActionResult>
  VisitUsingDecl(sona::ref_ptr<AST::UsingDecl const> usingDecl) = 0;

  virtual sona::owner<ActionResult>
  VisitFuncDecl(sona::ref_ptr<AST::FuncDecl const> funcDecl) = 0;

  virtual sona::owner<ActionResult>
  VisitVarDecl(sona::ref_ptr<AST::VarDecl const> varDecl) = 0;
};

class TypeVisitor {
public:
  virtual sona::owner<ActionResult>
  VisitBuiltinType(sona::ref_ptr<AST::BuiltinType const> builtinType) = 0;

  virtual sona::owner<ActionResult>
  VisitTupleType(sona::ref_ptr<AST::TupleType const> tupleType) = 0;

  virtual sona::owner<ActionResult>
  VisitArrayType(sona::ref_ptr<AST::ArrayType const> arrayType) = 0;

  virtual sona::owner<ActionResult>
  VisitPointerType(sona::ref_ptr<AST::PointerType const> ptrType) = 0;

  virtual sona::owner<ActionResult>
  VisitLValueRefType(sona::ref_ptr<AST::LValueRefType const> lvRefType) = 0;

  virtual sona::owner<ActionResult>
  VisitRValueRefType(sona::ref_ptr<AST::RValueRefType const> rvRefType) = 0;

  virtual sona::owner<ActionResult>
  VisitFunctionType(sona::ref_ptr<AST::FunctionType const> funcType) = 0;

  virtual sona::owner<ActionResult>
  VisitClassType(sona::ref_ptr<AST::ClassType const> classType) = 0;

  virtual sona::owner<ActionResult>
  VisitEnumType(sona::ref_ptr<AST::EnumType const> enumType) = 0;

  virtual sona::owner<ActionResult>
  VisitEnumClassType(sona::ref_ptr<AST::EnumClassType const> enumClassType) = 0;

  virtual sona::owner<ActionResult>
  VisitUsingType(sona::ref_ptr<AST::UsingType const> usingType) = 0;
};

} // namespace Backend
} // namespace ckx

#endif // ASTVISITOR_H
