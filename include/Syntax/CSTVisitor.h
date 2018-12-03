#ifndef CSTVISITOR_H
#define CSTVISITOR_H

#include <sona/result_wrapper.h>
#include <sona/pointer_plus.hpp>
#include <type_traits>

#include <Syntax/CSTFwd.h>

namespace ckx {
namespace Syntax {

using MiscResult = sona::result_wrapper;
using DeclResult = sona::result_wrapper;
using TypeResult = sona::result_wrapper;
using ExprResult = sona::result_wrapper;
using StmtResult = sona::result_wrapper;

class CSTMiscVisitor {
public:
  virtual MiscResult
  VisitAttributeList(sona::ref_ptr<AttributeList> attributeList) = 0;
  virtual MiscResult VisitImport(sona::ref_ptr<Import> imported) = 0;
  virtual MiscResult VisitExport(sona::ref_ptr<Export> exported) = 0;
};

class CSTTypeVisitor {
public:
  virtual TypeResult VisitBasicType(sona::ref_ptr<BasicType> type) = 0;
  virtual TypeResult
  VisitUserDefinedType(sona::ref_ptr<UserDefinedType> type) = 0;
  virtual TypeResult
  VisitTemplatedType(sona::ref_ptr<TemplatedType> type) = 0;
  virtual TypeResult
  VisitComposedType(sona::ref_ptr<ComposedType> type);
};

class CSTDeclVisitor {
public:
  virtual DeclResult VisitForwardDecl(sona::ref_ptr<ForwardDecl> decl) = 0;
  virtual DeclResult VisitClassDecl(sona::ref_ptr<ClassDecl> decl) = 0;
  virtual DeclResult VisitEnumDecl(sona::ref_ptr<EnumDecl> decl) = 0;
  virtual DeclResult VisitFuncDecl(sona::ref_ptr<FuncDecl> decl) = 0;
  virtual DeclResult VisitADTDecl(sona::ref_ptr<ADTDecl> decl) = 0;
};

class CSTStmtVisitor {
public:
  virtual StmtResult VisitEmptyStmt(sona::ref_ptr<EmptyStmt> stmt) = 0;
  virtual StmtResult VisitExprStmt(sona::ref_ptr<ExprStmt> stmt) = 0;
  virtual StmtResult VisitIfStmt(sona::ref_ptr<IfStmt> stmt) = 0;
  virtual StmtResult VisitMatchStmt(sona::ref_ptr<MatchStmt> stmt) = 0;
  virtual StmtResult VisitForStmt(sona::ref_ptr<ForStmt> stmt) = 0;
  virtual StmtResult VisitForEachStmt(sona::ref_ptr<ForEachStmt> stmt) = 0;
  virtual StmtResult VisitWhileStmt(sona::ref_ptr<WhileStmt> stmt) = 0;
  virtual StmtResult VisitCompoundStmt(sona::ref_ptr<CompoundStmt> stmt) = 0;
  virtual StmtResult VisitReturnStmt(sona::ref_ptr<ReturnStmt> stmt) = 0;
};

class CSTExprVisitor {

};

} // namespace Syntax
} // namespace ckx

#endif // CSTVISITOR_H
