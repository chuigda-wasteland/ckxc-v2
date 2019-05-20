#ifndef ASTVISITOR_H
#define ASTVISITOR_H

#include "AST/Decl.hpp"
#include "AST/Stmt.hpp"
#include "AST/Expr.hpp"

#include "sona/pointer_plus.hpp"
#include "sona/util.hpp"

namespace ckx {
namespace Backend {

class DeclResult {
public:
  virtual ~DeclResult() = 0;
};

template <typename T>
class DeclResultImpl : public DeclResult {
public:
  DeclResultImpl(T&& value) : m_Value(std::forward<T>(value)) {}
  DeclResultImpl(DeclResultImpl const&) = delete;
  DeclResultImpl(DeclResultImpl&&) = delete;
  DeclResultImpl& operator=(DeclResultImpl const&) = delete;
  DeclResultImpl& operator=(DeclResultImpl &&) = delete;

  T const& GetValue() const noexcept { return m_Value; }

private:
  T m_Value;
};

class VoidType {};

template <typename T> sona::owner<DeclResult> CreateDeclResult(T&& t) {
  return new DeclResultImpl<T>(std::forward<T>(t));
}

class DeclVisitor {
public:
  virtual sona::owner<DeclResult>
  VisitTransUnit(sona::ref_ptr<AST::TransUnitDecl const> transUnitDecl) = 0;

  virtual sona::owner<DeclResult>
  VisitLabelDecl(sona::ref_ptr<AST::LabelDecl const> labelDecl) = 0;

  virtual sona::owner<DeclResult>
  VisitClassDecl(sona::ref_ptr<AST::ClassDecl const> classDecl) = 0;

  virtual sona::owner<DeclResult>
  VisitEnumDecl(sona::ref_ptr<AST::EnumDecl const> enumDecl) = 0;

  virtual sona::owner<DeclResult>
  VisitEnumeratorDecl(
      sona::ref_ptr<AST::EnumeratorDecl const> enumeratorDecl) = 0;

  virtual sona::owner<DeclResult>
  VisitEnumClassInternDecl(
      sona::ref_ptr<AST::EnumClassInternDecl const> eciDecl) = 0;

  virtual sona::owner<DeclResult>
  VisitEnumClassDecl(sona::ref_ptr<AST::EnumClassDecl const> enumClassDecl) = 0;

  virtual sona::owner<DeclResult>
  VisitUsingDecl(sona::ref_ptr<AST::UsingDecl const> usingDecl) = 0;

  virtual sona::owner<DeclResult>
  VisitFuncDecl(sona::ref_ptr<AST::FuncDecl const> funcDecl) = 0;

  virtual sona::owner<DeclResult>
  VisitVarDecl(sona::ref_ptr<AST::VarDecl const> varDecl) = 0;
};

} // namespace Backend
} // namespace ckx

#endif // ASTVISITOR_H
