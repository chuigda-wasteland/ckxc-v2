#ifndef ASTCONTEXT_HPP
#define ASTCONTEXT_HPP

#include "Type.hpp"
#include "sona/pointer_plus.hpp"
#include <unordered_set>

namespace ckx {

/// @todo Finish AST Context
class ASTContext {
public:
    ASTContext() = default;
    ~ASTContext() = default;

private:
    template <typename Type_t>
    using TypeSet =
    std::unordered_set<Type_t, TypeHash<Type_t>, TypeEqual>;

    TypeSet<TupleType> m_TupleTypes;
    TypeSet<ArrayType> m_ArrayTypes;
    TypeSet<PointerType> m_PointerTypes;
    TypeSet<RefType> m_RefTypes;
    TypeSet<FunctionType> m_FuncTypes;
};

} // namespace ckx

#endif // ASTCONTEXT_HPP
