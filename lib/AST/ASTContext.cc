#include "AST/ASTContext.h"

namespace ckx {
namespace AST {

QualType
ASTContext::GetBuiltinType(BuiltinType::BuiltinTypeId btid) const noexcept {
  switch (btid) {
  #define BUILTIN_TYPE(name, rep, size, isint, \
                       issigned, signedver, unsignedver) \
    case BuiltinType::BTI_##name: { \
      static BuiltinType name##Type{ BuiltinType::BTI_##name }; \
      return QualType(&(name##Type)); \
    }
  #include "Syntax/BuiltinTypes.def"
  }
}

QualType ASTContext::AddUserDefinedType(sona::owner<Type>&& type) {
  /// @note It's safe to use a vector, since all user defined types shall be
  /// added exactly once.
  sona::ref_ptr<Type> borrowed_value = type.borrow();
  m_UserDefinedTypes.push_back(std::move(type));

  return QualType(borrowed_value);
}

QualType ASTContext::CreateTupleType(TupleType::TupleElements_t &&elems) {
  auto iter = m_TupleTypes.emplace(std::move(elems)).first;
  return QualType(sona::ref_ptr<Type const>(*iter));
}

QualType ASTContext::CreateArrayType(QualType base, size_t size) {
  auto iter = m_ArrayTypes.emplace(base, size).first;
  return QualType(sona::ref_ptr<Type const>(*iter));
}

QualType ASTContext::CreatePointerType(QualType pointee) {
  auto iter = m_PointerTypes.emplace(pointee).first;
  return QualType(sona::ref_ptr<Type const>(*iter));
}

QualType ASTContext::CreateLValueRefType(QualType referenced) {
  auto iter = m_LValueRefTypes.emplace(referenced).first;
  return QualType(*iter);
}

QualType ASTContext::CreateRValueRefType(QualType referenced) {
  auto iter = m_RValueRefTypes.emplace(referenced).first;
  return QualType(*iter);
}

QualType ASTContext::BuildFunctionType(
    std::vector<QualType> &&paramTypes, QualType retType) {
  auto iter = m_FuncTypes.emplace(std::move(paramTypes), retType).first;
  return QualType(sona::ref_ptr<Type const>(*iter));
}

} // namespace AST
} // namespace ckx
