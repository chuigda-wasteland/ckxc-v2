#ifndef EXPRBASE_H
#define EXPRBASE_H

#include "ExprFwd.h"

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
    EI_Integral,
    EI_Floating,
    EI_Char,
    EI_String,
    EI_Bool,
    EI_Tuple,
    EI_Array,
    EI_Paren,
    EI_ExplicitCast,

    // Intermediate expressions created by Sema
    EI_ImplicitCast,
  };

  virtual ~Expr() = default;

protected:
  Expr(ExprId id) : m_ExprId(id) {}

  ExprId GetExprId() const { return m_ExprId; }

private:
  ExprId m_ExprId;
};

} // namespace AST
} // namespace ckx

#endif // EXPRBASE_H
