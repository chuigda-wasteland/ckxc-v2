#ifndef EXPRFWD_H
#define EXPRFWD_H

namespace ckx {
namespace AST {

/// @todo replace hand-written codes with tablegen
class Expr;
class AssignExpr;
class UnaryExpr;
class BinaryExpr;
class CondExpr;
class IdExpr;
class IntLiteralExpr;
class UIntLiteralExpr;
class FloatLiteralExpr;
class CharLiteralExpr;
class StringLiteralExpr;
class BoolLiteralExpr;
class NullptrLiteralExpr;
// class TupleLiteralExpr;
// class ArrayLiteralExpr;
class ParenExpr;

class ImplicitCast;
class ExplicitCastExpr;
class CastStep;

} // namespace AST
} // namespace ckx

#endif // EXPRFWD_H
