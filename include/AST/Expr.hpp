#ifndef EXPR_HPP
#define EXPR_HPP

#include "DeclBase.hpp"
#include "ExprBase.hpp"
#include "StmtBase.hpp"

#include "sona/either.hpp"
#include "sona/optional.hpp"
#include "sona/pointer_plus.hpp"
#include <type_traits>
#include <vector>

namespace ckx {
namespace AST {

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
  AssignExpr(AssignmentOperator op, sona::owner<Expr> &&assigned,
             sona::owner<Expr> &&assignee)
      : Expr(ExprId::EI_Assign), m_Operator(op),
        m_Assigned(std::move(assigned)), m_Assignee(std::move(assignee)) {}

  AssignmentOperator GetOperator() const noexcept { return m_Operator; }

  sona::ref_ptr<Expr const> GetAssigned() const noexcept {
    return m_Assigned.borrow();
  }

  sona::ref_ptr<Expr const> GetAssignee() const noexcept {
    return m_Assignee.borrow();
  }

private:
  AssignmentOperator m_Operator;
  sona::owner<Expr> m_Assigned, m_Assignee;
};

class UnaryExpr : public Expr {
public:
  UnaryExpr(UnaryOperator op, sona::owner<Expr> &&operand)
    : Expr(ExprId::EI_Unary), m_Operator(op),
      m_Operand(std::move(operand)) {}

  UnaryOperator GetOperator() const noexcept { return m_Operator; }

  sona::ref_ptr<Expr const> GetOperand() const noexcept {
    return m_Operand.borrow();
  }

private:
  UnaryOperator m_Operator;
  sona::owner<Expr> m_Operand;
};

class BinaryExpr : public Expr {
public:
  BinaryExpr(BinaryOperator op, sona::owner<Expr> &&leftOperand,
             sona::owner<Expr> &&rightOperand)
    : Expr(ExprId::EI_Binary), m_Operator(op),
      m_LeftOperand(std::move(leftOperand)),
      m_RightOperand(std::move(rightOperand)){}

  BinaryOperator GetOperator() const noexcept { return m_Operator; }

  sona::ref_ptr<Expr const> GetLeftOperand() const noexcept {
    return m_LeftOperand.borrow();
  }

  sona::ref_ptr<Expr const> GetRightOperand() const noexcept {
    return m_RightOperand.borrow();
  }

private:
  BinaryOperator m_Operator;
  sona::owner<Expr> m_LeftOperand, m_RightOperand;
};

class CondExpr : public Expr {
public:
  CondExpr(sona::owner<Expr> &&condExpr, sona::owner<Expr> &&thenExpr,
           sona::owner<Expr> &&elseExpr)
    : Expr(ExprId::EI_Cond), m_CondExpr(std::move(condExpr)),
      m_ThenExpr(std::move(thenExpr)), m_ElseExpr(std::move(elseExpr)) {}

private:
  sona::owner<Expr> m_CondExpr, m_ThenExpr, m_ElseExpr;
};

class IdExpr : public Expr {
public:
  IdExpr(sona::string_ref const& idString)
    : Expr(ExprId::EI_ID), m_IdString(idString) {}

  sona::string_ref const &GetIdString() const noexcept { return m_IdString; }

private:
  sona::string_ref m_IdString;
};

class LiteralExpr : public Expr {
protected:
  LiteralExpr(ExprId literalId) : Expr(literalId) {
    sona_assert(literalId >= ExprId::EI_Integral &&
                literalId <= ExprId::EI_String);
  }

private:
};

class IntegralLiteralExpr : public LiteralExpr {
public:
  template <typename Integer_t>
  IntegralLiteralExpr(Integer_t value)
      : LiteralExpr(ExprId::EI_Integral), m_Value(value) {
    static_assert(std::is_same<Integer_t, std::int64_t>::value ||
                      std::is_same<Integer_t, std::uint64_t>::value,
                  "Not an integer!");
  }

  bool IsSignedInt() const noexcept { return m_Value.contains_t1(); }

  bool IsUnsignedInt() const noexcept { return !IsSignedInt(); }

  std::int64_t GetAsSIntUnsafe() const noexcept {
    sona_assert(IsSignedInt());
    return m_Value.as_t1();
  }

  std::uint64_t GetAsUIntUnsafe() const noexcept {
    sona_assert(IsUnsignedInt());
    return m_Value.as_t2();
  }

private:
  sona::either<std::int64_t, std::uint64_t> m_Value;
};

class FloatingLiteralExpr : public LiteralExpr {
public:
  FloatingLiteralExpr(double value)
      : LiteralExpr(ExprId::EI_Floating),
        m_Value(value) {}

  double GetValue() const noexcept { return m_Value; }

private:
  double m_Value;
};

/// @todo implement char then
class CharLiteralExpr : public LiteralExpr {};

/// @todo implement string then
class StringLiteralExpr : public LiteralExpr {};

class TupleLiteralExpr : public Expr {
public:
  TupleLiteralExpr(std::vector<sona::owner<Expr>> &&elementExprs)
    : Expr(ExprId::EI_Tuple), m_ElementExprs(std::move(elementExprs)) {}

  auto GetElementExprs() const noexcept {
    return sona::linq::from_container(m_ElementExprs).
      transform([](sona::owner<Expr> const& it)
          { return it.borrow(); });
  }

private:
  std::vector<sona::owner<Expr>> m_ElementExprs;
};

class ArrayLiteralExpr : public Expr {
public:
  ArrayLiteralExpr(std::vector<sona::owner<Expr>> &&elementExprs)
    : Expr(ExprId::EI_Array), m_ElementExprs(std::move(elementExprs)) {}

  auto GetElementExprs() const noexcept {
    return sona::linq::from_container(m_ElementExprs).
      transform([](sona::owner<Expr> const& it)
    { return it.borrow(); });
  }

private:
  std::vector<sona::owner<Expr>> m_ElementExprs;
};

class ParenExpr : public Expr {
public:
  ParenExpr(sona::owner<Expr> &&expr)
    : Expr(ExprId::EI_Paren), m_Expr(std::move(expr)) {}

  sona::ref_ptr<Expr const> GetExpr() const noexcept { return m_Expr.borrow(); }

private:
  sona::owner<Expr> m_Expr;
};

} // namespace AST
} // namespace ckx

#endif // EXPR_HPP
