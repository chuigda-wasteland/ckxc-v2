#ifndef CSTFWD_H
#define CSTFWD_H

namespace ckx {
namespace Syntax {

class Node;
  class TransUnit;
  class AttributeList;
  class Identifier;
  class Import;
  class Export;

  class Type;
    class BasicType;
    class UserDefinedType;
    class TemplatedType;
    class ComposedType;

  class Decl;
    class TemplatedDecl;
    class ForwardDecl;
    class ClassDecl;
    class EnumDecl;
    class ADTDecl;
    class FuncDecl;
    class VarDecl;

  class Stmt;
    class EmptyStmt;
    class ExprStmt;
    class IfStmt;
    class MatchStmt;
    class ForStmt;
    class ForEachStmt;
    class WhileStmt;
    class CompoundStmt;
    class ReturnStmt;

  class Expr;
    class LiteralExpr;
    class StringLiteralExpr;
    class IdRefExpr;
    class FuncCallExpr;
    class UnaryExpr;
    class BinaryExpr;
    class MixFixExpr;

} // namespace Syntax
} // namespace ckx

#endif // CSTFWD_H
