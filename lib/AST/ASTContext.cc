#include "AST/ASTContext.hpp"

namespace ckx {
namespace AST {

sona::ref_ptr<Type const>
ASTContext::GetBuiltinType(BuiltinType::BuiltinTypeId btid) const noexcept {
  /// @todo consider use tablegen to generate this
#define HANDLE_SINGLE_BUILTIN_TYPE(TYID)                                       \
  case BuiltinType::BuiltinTypeId::BTI_##TYID: {                               \
    static BuiltinType TYID##Type{ BuiltinType::BuiltinTypeId::BTI_##TYID };   \
    return TYID##Type;                                                         \
  }

  switch (btid) {
    HANDLE_SINGLE_BUILTIN_TYPE(u8)
    HANDLE_SINGLE_BUILTIN_TYPE(u16)
    HANDLE_SINGLE_BUILTIN_TYPE(u32)
    HANDLE_SINGLE_BUILTIN_TYPE(u64)
    HANDLE_SINGLE_BUILTIN_TYPE(i8)
    HANDLE_SINGLE_BUILTIN_TYPE(i16)
    HANDLE_SINGLE_BUILTIN_TYPE(i32)
    HANDLE_SINGLE_BUILTIN_TYPE(i64)
    HANDLE_SINGLE_BUILTIN_TYPE(r32)
    HANDLE_SINGLE_BUILTIN_TYPE(r64)
    HANDLE_SINGLE_BUILTIN_TYPE(r128)
    HANDLE_SINGLE_BUILTIN_TYPE(bool)
    HANDLE_SINGLE_BUILTIN_TYPE(nil)
    HANDLE_SINGLE_BUILTIN_TYPE(void)
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
