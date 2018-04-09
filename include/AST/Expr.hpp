#ifndef EXPR_HPP
#define EXPR_HPP

#include "ExprBase.hpp"
#include "DeclBase.hpp"
#include "StmtBase.hpp"

#include "Basic/SourceRange.hpp"

#include "sona/pointer_plus.hpp"
#include "sona/optional.hpp"
#include <vector>

namespace ckx {

enum class UnaryOperator {
    UOP_Incr,
    UOP_Decr,
    UOP_Deref,
    UOP_AddrOf,
    UOP_Positive,
    UOP_Negative,
    UOP_LogicalNot,
    UOP_BitwiseNot
};

enum class BinaryOperator {
    BOP_Add,
    BOP_Sub,
    BOP_Mul,
    BOP_Div,
    BOP_Mod,

    BOP_LogicalAnd,
    BOP_LogicalOr,
    BOP_LogicalNot,
    BOP_LogicalXor,

    BOP_BitwiseAnd,
    BOP_BitwiseOr,
    BOP_BitwiseNot,
    BOP_BitwiseXor,

    BOP_LShift,
    BOP_RShift,

    BOP_Lt,
    BOP_Gt,
    BOP_Eq,
    BOP_Neq,
    BOP_Leq,
    BOP_Geq,

    BOP_AddAssign,
    BOP_SubAssign,
    BOP_MulAssign,
    BOP_DivAssign,
    BOP_ModAssign,

    BOP_BitwiseAndAssign,
    BOP_BitwiseOrAssign,
    BOP_BitwiseNotAssign,
    BOP_BitwiseXorAssign,

    BOP_BitwiseLShiftAssign,
    BOP_BitwiseRShiftAssign
};

class AssignExpr : public Expr {
public:

private:
    sona::owner<Expr> m_Assigned, m_Assignee;
    SourceLocation m_AssignLocation;
};

class UnaryExpr : public Expr {
private:
    UnaryOperator m_Operator;
    sona::owner<Expr> m_Operand;
    SourceLocation m_OperandLocation;
};

class BinaryExpr : public Expr {
private:
    BinaryOperator m_Operator;
    sona::owner<Expr> m_LeftOperand, m_RightOperand;
    SourceLocation m_OperandLocation;
};

class CondExpr : public Expr {
private:
    sona::owner<Expr> m_CondExpr, m_ThenExpr, m_ElseExpr;
    SourceLocation m_QuestionLocation, m_ColonLocation;
};

class IdExpr : public Expr {
private:
    std::string m_IdString;
    SourceRange m_IdRange;
};

class LiteralExpr : public Expr {
private:
    SourceRange m_LiteralRange;
};

class IntegralLiteralExpr : public LiteralExpr {
private:
    sona::either<std::int64_t, std::uint64_t> m_Value;
};

class FloatingLiteralExpr : public LiteralExpr {
private:
    double m_Value;
};

/// @todo implement char then
class CharLiteralExpr : public LiteralExpr {};

/// @todo implement string then
class StringLiteralExpr : public LiteralExpr {};

class TupleLiteralExpr : public Expr{
public:
private:
    std::vector<sona::owner<Expr>> m_ElementExprs;
    std::vector<SourceLocation> m_CommaLocations;
    SourceLocation m_LeftParenLocation, m_RightParenLocation;
};

/// I'm hungry! feed me something QwQ
class ArrayLiteralExpr : public Expr {
private:
    std::vector<sona::owner<Expr>> m_ElementExprs;
    std::vector<SourceLocation> m_CommaLocations;
    SourceLocation m_LeftBraceLocation, m_RightBraceLocation;
};

class BracedExpr : public Expr {
private:
    sona::owner<Expr> m_Expr;
    SourceLocation m_LeftParenLocation, m_RightParenLocation;
};

} // namespace ckx

#endif // EXPR_HPP
