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

  enum class ValueCat { VC_LValue, VC_RValue, VC_XValue };

  virtual ~Expr() = default;

  ExprId GetExprId() const noexcept { return m_ExprId; }

  QualType GetExprType() const noexcept { return m_ExprType; }

  ValueCat GetValueCat() const noexcept { return m_ValueCat; }

protected:
  Expr(ExprId id, QualType exprType, ValueCat valueCat)
    : m_ExprId(id), m_ExprType(exprType), m_ValueCat(valueCat) {}

private:
  ExprId m_ExprId;
  QualType m_ExprType;
  ValueCat m_ValueCat;
};

} // namespace AST
} // namespace ckx

#endif // EXPRBASE_H
