#include "AST/ASTContext.h"

namespace ckx {
namespace AST {

sona::ref_ptr<Type const>
ASTContext::GetBuiltinType(BuiltinType::BuiltinTypeId btid) const noexcept {
  switch (btid) {
  #define BUILTIN_TYPE(name, rep, size, isint, \
                       issigned, signedver, unsignedver) \
    case BuiltinType::BuiltinTypeId::BTI_##name: { \
      static BuiltinType name##Type{ BuiltinType::BuiltinTypeId::BTI_##name }; \
      return name##Type; \
    }
  #include "Syntax/BuiltinTypes.def"
  }
}

sona::ref_ptr<Type>
ASTContext::AddUserDefinedType(sona::owner<Type>&& type) {
  /// @note It's safe to use a vector, since all user defined types shall be
  /// added exactly once.
  sona::ref_ptr<Type> borrowed_value = type.borrow();
  m_UserDefinedTypes.push_back(std::move(type));

  return borrowed_value;
}

sona::ref_ptr<TupleType const>
ASTContext::CreateTupleType(TupleType::TupleElements_t &&elems) {
  auto iter = m_TupleTypes.emplace(std::move(elems)).first;
  return sona::ref_ptr<TupleType const>(*iter);
}

sona::ref_ptr<ArrayType const>
ASTContext::CreateArrayType(sona::ref_ptr<Type> base, size_t size) {
  auto iter = m_ArrayTypes.emplace(base, size).first;
  return sona::ref_ptr<ArrayType const>(*iter);
}

sona::ref_ptr<PointerType const>
ASTContext::CreatePointerType(sona::ref_ptr<Type const> pointee) {
  auto iter = m_PointerTypes.emplace(pointee).first;
  return sona::ref_ptr<PointerType const>(*iter);
}

sona::ref_ptr<LValueRefType const>
ASTContext::CreateLValueRefType(sona::ref_ptr<const Type> referenced) {
  auto iter = m_LValueRefTypes.emplace(referenced).first;
  return *iter;
}

sona::ref_ptr<RValueRefType const>
ASTContext::CreateRValueRefType(sona::ref_ptr<const Type> referenced) {
  auto iter = m_RValueRefTypes.emplace(referenced).first;
  return *iter;
}

} // namespace AST
} // namespace ckx
