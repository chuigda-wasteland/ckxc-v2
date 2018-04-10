#ifndef EXPRBASE_HPP
#define EXPRBASE_HPP

#include "ExprFwd.hpp"

namespace ckx {

class Expr {
public:
    enum class ExprId {
        EI_Unary,
        EI_Binary,
        EI_Assign,
        EI_Cond,
        EI_ID,
        EI_Integral,
        EI_Floating,
        EI_Char,
        EI_String,
        EI_Tuple,
        EI_Array
    };

protected:
    Expr(ExprId id) : m_ExprId(id) {}

    ExprId GetExprId() const { return m_ExprId; }

private:
    ExprId m_ExprId;
};

} // namespace ckx

#endif // EXPRBASE_HPP
