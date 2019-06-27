#ifndef EXPRBASE_H
#define EXPRBASE_H

#include "ExprFwd.h"
#include "TypeBase.h"

#include "sona/stringref.h"

namespace ckx {
namespace AST {

class Expr {
public:
  enum class ExprId {
    // Directly corresponds to syntactical structure
    EI_Unary,
    EI_Binary,
    EI_Assign,
    EI_Cond,
    EI_ID,
    EI_IntLiteral,
    EI_UIntLiteral,
    EI_FloatLiteral,
    EI_CharLiteral,
    EI_StringLiteral,
    EI_BoolLiteral,
    EI_NullptrLiteral,
    /// @todo see descriptsion on type AST::TupleType
    // EI_Tuple,
    EI_Array,
    EI_Paren,
    EI_ExplicitCast,

    // Intermediate expressions created by Sema
    EI_ImplicitCast,
  };

  virtual ~Expr() = default;

  ExprId GetExprId() const noexcept { return m_ExprId; }

  QualType GetExprType() const noexcept { return m_ExprType; }

protected:
  Expr(ExprId id, QualType exprType) : m_ExprId(id), m_ExprType(exprType) {}

private:
  ExprId m_ExprId;
  QualType m_ExprType;
};

} // namespace AST
} // namespace ckx

#endif // EXPRBASE_H
