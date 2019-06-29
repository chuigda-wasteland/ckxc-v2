#ifndef ASTCONTEXT_H
#define ASTCONTEXT_H

#include "Type.h"
#include "sona/pointer_plus.h"
#include <list>
#include <unordered_set>

namespace ckx {
namespace AST {

/// @todo Finish AST Context
class ASTContext {
public:
  ASTContext() = default;
  ~ASTContext() = default;

  QualType AddUserDefinedType(sona::owner<AST::Type> &&type);
  QualType CreateTupleType(TupleType::TupleElements_t &&elems);
  QualType CreateArrayType(QualType base, size_t size);
  QualType CreatePointerType(QualType pointee);
  QualType CreateLValueRefType(QualType referenced);
  QualType CreateRValueRefType(QualType referenced);
  QualType BuildFunctionType(std::vector<QualType> &&paramTypes,
                             QualType retType);
  QualType GetBuiltinType(BuiltinType::BuiltinTypeId btid) const noexcept;

private:
  template <typename Type_t>
  using TypeSet = std::unordered_set<Type_t, TypeHash<Type_t>, TypeEqual>;

  /// @todo must we keep strong reference right here?
  std::list<sona::owner<Type>> m_UserDefinedTypes;
  TypeSet<TupleType> m_TupleTypes;
  TypeSet<ArrayType> m_ArrayTypes;
  TypeSet<PointerType> m_PointerTypes;
  TypeSet<LValueRefType> m_LValueRefTypes;
  TypeSet<RValueRefType> m_RValueRefTypes;
  TypeSet<FunctionType> m_FuncTypes;
};

} // namespace AST
} // namespace ckx

#endif // ASTCONTEXT_H
