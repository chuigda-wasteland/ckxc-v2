#ifndef CSTVISITOR_H
#define CSTVISITOR_H

#include <sona/result_wrapper.h>
#include <sona/pointer_plus.hpp>
#include <type_traits>

#include <Syntax/CSTFwd.h>

namespace ckx {
namespace Syntax {

template <typename ResultType>
class CSTMiscVisitor {
public:
  virtual ResultType
  VisitAttributeList(sona::ref_ptr<AttributeList> attributeList) = 0;
  virtual ResultType VisitImport(sona::ref_ptr<Import> imported) = 0;
  virtual ResultType VisitExport(sona::ref_ptr<Export> exported) = 0;
};

template <typename ResultType>
class CSTTypeVisitor {
public:
  virtual ResultType VisitBasicType(sona::ref_ptr<BasicType> type) = 0;
  virtual ResultType
  VisitUserDefinedType(sona::ref_ptr<UserDefinedType> type) = 0;
  virtual ResultType
  VisitTemplatedType(sona::ref_ptr<TemplatedType> type) = 0;
  virtual ResultType
  VisitComposedType(sona::ref_ptr<ComposedType> type);
};

template <typename ResultType>
class CSTDeclVisitor {
public:
  virtual ResultType VisitForwardDecl(sona::ref_ptr<ForwardDecl> decl) = 0;
  virtual ResultType VisitClassDecl(sona::ref_ptr<ClassDecl> decl) = 0;
  virtual ResultType VisitEnumDecl(sona::ref_ptr<EnumDecl> decl) = 0;
  virtual ResultType VisitFuncDecl(sona::ref_ptr<FuncDecl> decl) = 0;
  virtual ResultType VisitADTDecl(sona::ref_ptr<ADTDecl> decl) = 0;
};

template <typename ResultType>
class CSTStmtVisitor {
public:
  virtual ResultType VisitEmptyStmt(sona::ref_ptr<EmptyStmt> stmt) = 0;
  virtual ResultType VisitExprStmt(sona::ref_ptr<ExprStmt> stmt) = 0;
  virtual ResultType VisitIfStmt(sona::ref_ptr<IfStmt> stmt) = 0;
  virtual ResultType VisitMatchStmt(sona::ref_ptr<MatchStmt> stmt) = 0;
  virtual ResultType VisitForStmt(sona::ref_ptr<ForStmt> stmt) = 0;
  virtual ResultType VisitForEachStmt(sona::ref_ptr<ForEachStmt> stmt) = 0;
  virtual ResultType VisitWhileStmt(sona::ref_ptr<WhileStmt> stmt) = 0;
  virtual ResultType VisitCompoundStmt(sona::ref_ptr<CompoundStmt> stmt) = 0;
  virtual ResultType VisitReturnStmt(sona::ref_ptr<ReturnStmt> stmt) = 0;
};

template <typename ResultType>
class CSTExprVisitor {
public:
  virtual ResultType VisitLiteralExpr(sona::ref_ptr<LiteralExpr> expr) = 0;
  virtual ResultType
  VisitStringLiteralExpr(sona::ref_ptr<StringLiteralExpr> expr) = 0;
  virtual ResultType VisitIdRefExpr(sona::ref_ptr<IdRefExpr> expr) = 0;
  virtual ResultType VisitFuncCallExpr(sona::ref_ptr<FuncCallExpr> expr) = 0;
  virtual ResultType VisitUnaryExpr(sona::ref_ptr<UnaryExpr> expr) = 0;
  virtual ResultType VisitBinaryExpr(sona::ref_ptr<BinaryExpr> expr) = 0;
  virtual ResultType VisitMixFixExpr(sona::ref_ptr<MixFixExpr> expr) = 0;
};

template <typename ResultType>
class CSTTransUnitVisitor {
public:
  virtual ResultType VisitTransUnit(sona::ref_ptr<TransUnit> TransUnit) = 0;
};

} // namespace Syntax
} // namespace ckx

#endif // CSTVISITOR_H
