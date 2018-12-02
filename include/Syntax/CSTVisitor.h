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
  VisitAttributeList(sona::ref_ptr<CSTAttributeList> attributeList) = 0;
  virtual MiscResult VisitImport(sona::ref_ptr<CSTImport> imported) = 0;
  virtual MiscResult VisitExport(sona::ref_ptr<CSTExport> exported) = 0;
};

class CSTTypeVisitor {
public:
  virtual TypeResult VisitBasicType(sona::ref_ptr<CSTBasicType> type) = 0;
  virtual TypeResult
  VisitUserDefinedType(sona::ref_ptr<CSTUserDefinedType> type) = 0;
  virtual TypeResult
  VisitTemplatedType(sona::ref_ptr<CSTTemplatedType> type) = 0;
  virtual TypeResult
  VisitComposedType(sona::ref_ptr<CSTComposedType> type);
};

class CSTDeclVisitor {
public:
  virtual DeclResult VisitForwardDecl(sona::ref_ptr<CSTForwardDecl> decl) = 0;
  virtual DeclResult VisitClassDecl(sona::ref_ptr<CSTClassDecl> decl) = 0;
  virtual DeclResult VisitEnumDecl(sona::ref_ptr<CSTEnumDecl> decl) = 0;
  virtual DeclResult VisitFuncDecl(sona::ref_ptr<CSTFuncDecl> decl) = 0;
  virtual DeclResult VisitADTDecl(sona::ref_ptr<CSTADTDecl> decl) = 0;
};

class CSTStmtVisitor {
public:
  virtual StmtResult VisitEmptyStmt(sona::ref_ptr<CSTEmptyStmt> stmt) = 0;
  virtual StmtResult VisitExprStmt(sona::ref_ptr<CSTExprStmt> stmt) = 0;
  virtual StmtResult VisitIfStmt(sona::ref_ptr<CSTIfStmt> stmt) = 0;
  virtual StmtResult VisitMatchStmt(sona::ref_ptr<CSTMatchStmt> stmt) = 0;
  virtual StmtResult VisitForStmt(sona::ref_ptr<CSTForStmt> stmt) = 0;
  virtual StmtResult VisitForEachStmt(sona::ref_ptr<CSTForEachStmt> stmt) = 0;
  virtual StmtResult VisitWhileStmt(sona::ref_ptr<CSTWhileStmt> stmt) = 0;
  virtual StmtResult VisitCompoundStmt(sona::ref_ptr<CSTCompoundStmt> stmt) = 0;
  virtual StmtResult VisitReturnStmt(sona::ref_ptr<CSTReturnStmt> stmt) = 0;
};

class CSTExprVisitor {

};

} // namespace Syntax
} // namespace ckx

#endif // CSTVISITOR_H
