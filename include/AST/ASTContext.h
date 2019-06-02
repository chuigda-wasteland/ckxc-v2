#ifndef ASTCONTEXT_HPP
#define ASTCONTEXT_HPP

#include "Type.hpp"
#include "sona/pointer_plus.hpp"
#include <list>
#include <unordered_set>

namespace ckx {
namespace AST {

/// @todo Finish AST Context
class ASTContext {
public:
  ASTContext() = default;
  ~ASTContext() = default;

  sona::ref_ptr<Type> AddUserDefinedType(sona::owner<AST::Type> &&type);

  sona::ref_ptr<TupleType const>
  CreateTupleType(TupleType::TupleElements_t &&elems);

  sona::ref_ptr<ArrayType const> CreateArrayType(sona::ref_ptr<Type> base,
                                                 size_t size);

  sona::ref_ptr<PointerType const>
  CreatePointerType(sona::ref_ptr<Type const> pointee);

  sona::ref_ptr<LValueRefType const>
  CreateLValueRefType(sona::ref_ptr<Type const> referenced);

  sona::ref_ptr<FunctionType const>
  BuildFunctionType(std::vector<sona::ref_ptr<Type const>> const& paramTypes,
                    sona::ref_ptr<Type const> retType);

  sona::ref_ptr<Type const>
  GetBuiltinType(BuiltinType::BuiltinTypeId btid) const noexcept;

  sona::ref_ptr<RValueRefType const>
  CreateRValueRefType(sona::ref_ptr<Type const> referenced);

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

#endif // ASTCONTEXT_HPP
