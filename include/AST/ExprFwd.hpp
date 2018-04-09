#ifndef EXPRFWD_HPP
#define EXPRFWD_HPP

namespace ckx {

class Expr;
    class AssignExpr;
    class UnaryExpr;
    class BinaryExpr;
    class CondExpr;
    class IdExpr;
    class LiteralExpr;
        class IntegralLiteralExpr;
        class FloatingLiteralExpr;
        class CharLiteralExpr;
        class StringLiteralExpr;
    class TupleLiteralExpr;
    class ArrayLiteralExpr;
    class BracedExpr;
} // namespace ckx

#endif // EXPRFWD_HPP
