#ifndef EXPR_HPP
#define EXPR_HPP

#include "ExprBase.hpp"
#include "DeclBase.hpp"
#include "StmtBase.hpp"

#include "Basic/SourceRange.hpp"

#include "sona/pointer_plus.hpp"
#include "sona/optional.hpp"
#include <vector>
#include <type_traits>

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
    BOP_Geq
};

enum class AssignmentOperator {
    AOP_Assign,

    AOP_AddAssign,
    AOP_SubAssign,
    AOP_MulAssign,
    AOP_DivAssign,
    AOP_ModAssign,

    AOP_BitwiseAndAssign,
    AOP_BitwiseOrAssign,
    AOP_BitwiseNotAssign,
    AOP_BitwiseXorAssign,

    AOP_BitwiseLShiftAssign,
    AOP_BitwiseRShiftAssign
};

class AssignExpr : public Expr {
public:
    AssignExpr(AssignmentOperator op,
               sona::owner<Expr> &&assigned,
               sona::owner<Expr> &&assignee,
               SourceLocation assignLocation) :
        Expr(ExprId::EI_Assign),
        m_Operator(op),
        m_Assigned(std::move(assigned)),
        m_Assignee(std::move(assignee)),
        m_AssignLocation(assignLocation) {}

    AssignmentOperator GetOperator() const noexcept {
        return m_Operator;
    }

    sona::ref_ptr<Expr const> GetAssigned() const noexcept {
        return m_Assigned.borrow();
    }

    sona::ref_ptr<Expr const> GetAssignee() const noexcept {
        return m_Assignee.borrow();
    }

    SourceLocation GetAssignLocation() const noexcept {
        return m_AssignLocation;
    }

private:
    AssignmentOperator m_Operator;
    sona::owner<Expr> m_Assigned, m_Assignee;
    SourceLocation m_AssignLocation;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(UnaryOperator op,
              sona::owner<Expr> &&operand,
              SourceLocation operatorLocation)
        : Expr(ExprId::EI_Unary),
          m_Operator(op),
          m_Operand(std::move(operand)),
          m_OperatorLocation(operatorLocation) {}

    UnaryOperator GetOperator() const noexcept {
        return m_Operator;
    }

    sona::ref_ptr<Expr const> GetOperand() const noexcept {
        return m_Operand.borrow();
    }

    SourceLocation GetOperatorLocation() const noexcept {
        return m_OperatorLocation;
    }

private:
    UnaryOperator m_Operator;
    sona::owner<Expr> m_Operand;
    SourceLocation m_OperatorLocation;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(BinaryOperator op,
               sona::owner<Expr> &&leftOperand,
               sona::owner<Expr> &&rightOperand,
               SourceLocation operatorLocation)
        : Expr(ExprId::EI_Binary),
          m_Operator(op),
          m_LeftOperand(std::move(leftOperand)),
          m_RightOperand(std::move(rightOperand)),
          m_OperatorLocation(operatorLocation) {}

    BinaryOperator GetOperator() const noexcept {
        return m_Operator;
    }

    sona::ref_ptr<Expr const> GetLeftOperand() const noexcept {
        return m_LeftOperand.borrow();
    }

    sona::ref_ptr<Expr const> GetRightOperand() const noexcept {
        return m_RightOperand.borrow();
    }

    SourceLocation GetOperatorLocation() const noexcept {
        return m_OperatorLocation;
    }

private:
    BinaryOperator m_Operator;
    sona::owner<Expr> m_LeftOperand, m_RightOperand;
    SourceLocation m_OperatorLocation;
};

class CondExpr : public Expr {
public:
    CondExpr(sona::owner<Expr> &&condExpr,
             sona::owner<Expr> &&thenExpr,
             sona::owner<Expr> &&elseExpr,
             SourceLocation questionLocation,
             SourceLocation colonLocation)
        : Expr(ExprId::EI_Cond),
          m_CondExpr(std::move(condExpr)),
          m_ThenExpr(std::move(thenExpr)),
          m_ElseExpr(std::move(elseExpr)),
          m_QuestionLocation(questionLocation),
          m_ColonLocation(colonLocation) {}

private:
    sona::owner<Expr> m_CondExpr, m_ThenExpr, m_ElseExpr;
    SourceLocation m_QuestionLocation, m_ColonLocation;
};

class IdExpr : public Expr {
public:
    IdExpr(std::string &&idString, SourceRange &&idRange)
        : Expr(ExprId::EI_ID),
          m_IdString(std::move(idString)),
          m_IdRange(std::move(idRange)) {}

    std::string const& GetIdString() const noexcept {
        return m_IdString;
    }

    SourceRange const& GetIdRange() const noexcept {
        return m_IdRange;
    }

private:
    std::string m_IdString;
    SourceRange m_IdRange;
};

class LiteralExpr : public Expr {
public:
    SourceRange const& GetLiteralRange() const noexcept {
        return m_LiteralRange;
    }

protected:
    LiteralExpr(ExprId literalId, SourceRange &&literalRange)
        : Expr(literalId),
          m_LiteralRange(std::move(literalRange)) {
        sona_assert(literalId >= ExprId::EI_Integral \
                    && literalId <= ExprId::EI_String);
    }

private:
    SourceRange m_LiteralRange;
};

class IntegralLiteralExpr : public LiteralExpr {
public:
    template <typename Integer_t>
    IntegralLiteralExpr(Integer_t value, SourceRange &&range)
        : LiteralExpr(ExprId::EI_Integral, std::move(range)),
          m_Value(value) {
        static_assert(std::is_same<Integer_t, std::int64_t>::value
                      || std::is_same<Integer_t, std::uint64_t>::value,
                      "Not an integer!");
    }

    bool IsSignedInt() const noexcept {
        return m_Value.contains_t1();
    }

    bool IsUnsignedInt() const noexcept {
        return !IsSignedInt();
    }

    std::int64_t GetAsSInt() const noexcept {
        sona_assert(IsSignedInt());
        return m_Value.as_t1();
    }

    std::uint64_t GetAsUInt() const noexcept {
        sona_assert(IsUnsignedInt());
        return m_Value.as_t2();
    }

private:
    sona::either<std::int64_t, std::uint64_t> m_Value;
};

class FloatingLiteralExpr : public LiteralExpr {
public:
    FloatingLiteralExpr(double value, SourceRange &&literalRange) :
        LiteralExpr(ExprId::EI_Floating, std::move(literalRange)),
        m_Value(value) {}

    double GetValue() const noexcept {
        return m_Value;
    }

private:
    double m_Value;
};

/// @todo implement char then
class CharLiteralExpr : public LiteralExpr {};

/// @todo implement string then
class StringLiteralExpr : public LiteralExpr {};

class TupleLiteralExpr : public Expr{
public:
    TupleLiteralExpr(std::vector<sona::owner<Expr>> &&elementExprs,
                     std::vector<SourceLocation> &&commaLocations,
                     SourceLocation leftParenLocation,
                     SourceLocation rightParenLocation)
        : Expr(ExprId::EI_Tuple),
          m_ElementExprs(std::move(elementExprs)),
          m_CommaLocations(std::move(commaLocations)),
          m_LeftParenLocation(leftParenLocation),
          m_RightParenLocation(rightParenLocation) {}

    /** @todo ??? GetElementExprs() const noexcept; */

    std::vector<SourceLocation> const& GetCommaLocations() const noexcept {
        return m_CommaLocations;
    }

    SourceLocation GetLeftParenLocation() const noexcept {
        return m_LeftParenLocation;
    }

    SourceLocation GetRightParenLocation() const noexcept {
        return m_RightParenLocation;
    }

private:
    std::vector<sona::owner<Expr>> m_ElementExprs;
    std::vector<SourceLocation> m_CommaLocations;
    SourceLocation m_LeftParenLocation, m_RightParenLocation;
};

class ArrayLiteralExpr : public Expr {
public:
    ArrayLiteralExpr(std::vector<sona::owner<Expr>> &&elementExprs,
                     std::vector<SourceLocation> &&commaLocations,
                     SourceLocation leftBraceLocation,
                     SourceLocation rightBraceLocation)
        : Expr(ExprId::EI_Array),
          m_ElementExprs(std::move(elementExprs)),
          m_CommaLocations(std::move(commaLocations)),
          m_LeftBraceLocation(leftBraceLocation),
          m_RightBraceLocation(rightBraceLocation) {}

    /** @todo ??? GetElementExprs() const noexcept; */

    std::vector<SourceLocation> const& GetCommaLocations() const noexcept {
        return m_CommaLocations;
    }

    SourceLocation GetLeftBraceLocation() const noexcept {
        return m_LeftBraceLocation;
    }

    SourceLocation GetRightBraceLocation() const noexcept {
        return m_RightBraceLocation;
    }

private:
    std::vector<sona::owner<Expr>> m_ElementExprs;
    std::vector<SourceLocation> m_CommaLocations;
    SourceLocation m_LeftBraceLocation, m_RightBraceLocation;
};

class ParenExpr : public Expr {
public:
    ParenExpr(sona::owner<Expr> &&expr,
              SourceLocation leftParenLocation,
              SourceLocation rightParenLocation)
        : Expr(ExprId::EI_Paren),
          m_Expr(std::move(expr)),
          m_LeftParenLocation(leftParenLocation),
          m_RightParenLocation(rightParenLocation) {}

    sona::ref_ptr<Expr const> GetExpr() const noexcept {
        return m_Expr.borrow();
    }

    SourceLocation GetLeftParenLocation() const noexcept {
        return m_LeftParenLocation;
    }

    SourceLocation GetRightParenLocation() const noexcept {
        return m_RightParenLocation;
    }

private:
    sona::owner<Expr> m_Expr;
    SourceLocation m_LeftParenLocation, m_RightParenLocation;
};

} // namespace ckx

#endif // EXPR_HPP
