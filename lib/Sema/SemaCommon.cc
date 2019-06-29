#include "Sema/SemaCommon.h"

#include <limits>

namespace ckx {
namespace Sema {

AST::BuiltinType::BuiltinTypeId
SemaCommon::ClassifyBuiltinTypeId(int64_t i) noexcept {
  if (i <= std::numeric_limits<std::int8_t>::max()
      && i >= std::numeric_limits<std::int8_t>::min()) {
    return AST::BuiltinType::BuiltinTypeId::BTI_Int8;
  }
  else if (i <= std::numeric_limits<std::int16_t>::max()
           && i >= std::numeric_limits<std::int16_t>::min()) {
    return AST::BuiltinType::BuiltinTypeId::BTI_Int16;
  }
  else if (i <= std::numeric_limits<std::int32_t>::max()
           && i >= std::numeric_limits<std::int32_t>::min()) {
    return AST::BuiltinType::BuiltinTypeId::BTI_Int32;
  }
  else {
    return AST::BuiltinType::BuiltinTypeId::BTI_Int64;
  }
}

AST::BuiltinType::BuiltinTypeId
SemaCommon::ClassifyBuiltinTypeId(uint64_t u) noexcept {
  if (u <= std::numeric_limits<std::uint8_t>::max()) {
    return AST::BuiltinType::BuiltinTypeId::BTI_UInt8;
  }
  else if (u <= std::numeric_limits<std::uint16_t>::max()) {
    return AST::BuiltinType::BuiltinTypeId::BTI_UInt16;
  }
  else if (u <= std::numeric_limits<std::uint32_t>::max()) {
    return AST::BuiltinType::BuiltinTypeId::BTI_UInt32;
  }
  else {
    return AST::BuiltinType::BuiltinTypeId::BTI_UInt64;
  }
}

AST::BuiltinType::BuiltinTypeId
SemaCommon::ClassifyBuiltinTypeId(double f) noexcept {
  if (f <= std::numeric_limits<float>::max()
      && f >= std::numeric_limits<float>::min()) {
    return AST::BuiltinType::BuiltinTypeId::BTI_Float;
  }
  else {
    return AST::BuiltinType::BuiltinTypeId::BTI_Double;
  }
}

AST::BuiltinType::BuiltinTypeId
SemaCommon::CommmonBuiltinTypeId(
    AST::BuiltinType::BuiltinTypeId ty1id,
    AST::BuiltinType::BuiltinTypeId ty2id) noexcept {
  if (ty1id == ty2id) {
    return ty1id;
  }

  if (AST::BuiltinType::IsIntegral(ty1id)
      && AST::BuiltinType::IsIntegral(ty2id)) {
    if (AST::BuiltinType::IsSigned(ty1id)
        && AST::BuiltinType::IsSigned(ty2id)) {
      return std::max(
            ty1id, ty2id, [](auto a, auto b) {
              return AST::BuiltinType::SignedIntRank(a)
                     < AST::BuiltinType::SignedIntRank(b);
            });
    }
    else if (AST::BuiltinType::IsUnsigned(ty1id)
             && AST::BuiltinType::IsUnsigned(ty2id)) {
      return std::max(
            ty1id, ty2id, [](auto a, auto b) {
              return AST::BuiltinType::UnsignedIntRank(a)
                     < AST::BuiltinType::UnsignedIntRank(b);
            });
    }
    else {
      return AST::BuiltinType::BuiltinTypeId::BTI_NoType;
    }
  }
  else if (AST::BuiltinType::IsFloating(ty1id)
           && AST::BuiltinType::IsFloating(ty2id)) {
    return std::max(
          ty1id, ty2id, [](auto a, auto b) {
            return AST::BuiltinType::FloatRank(a)
                   < AST::BuiltinType::FloatRank(b);
          });
  }

  return AST::BuiltinType::BuiltinTypeId::BTI_NoType;
}

void SemaCommon::PushDeclContext(sona::ref_ptr<AST::DeclContext> context) {
  m_DeclContexts.push_back(context);
}

void SemaCommon::PopDeclContext() {
  m_DeclContexts.pop_back();
}

sona::ref_ptr<AST::DeclContext> SemaCommon::GetCurrentDeclContext() {
  return m_DeclContexts.back();
}

std::shared_ptr<Scope> const& SemaCommon::GetCurrentScope() const noexcept {
  return m_CurrentScope;
}

std::shared_ptr<Scope> const& SemaCommon::GetGlobalScope() const noexcept {
  return m_GlobalScope;
}

void SemaCommon::PushScope(Scope::ScopeFlags flags) {
  std::shared_ptr<Scope> newScope(new Scope(GetCurrentScope(), flags));
  if (GetCurrentScope() == nullptr) {
    m_GlobalScope = newScope;
  }
  m_CurrentScope = newScope;
}

void SemaCommon::PopScope() {
  m_CurrentScope = m_CurrentScope->GetParentScope();
}

AST::QualType SemaCommon::ResolveBuiltinTypeImpl(
    sona::ref_ptr<const Syntax::BuiltinType> basicType) {
  AST::BuiltinType::BuiltinTypeId bid;
  /// @todo consider use tablegen to generate this, or unify the two
  /// "type kinds" enumeration
  switch (basicType->GetTypeKind()) {
  #define BUILTIN_TYPE(name, rep, size, isint, \
                       issigned, signedver, unsignedver) \
    case Syntax::BuiltinType::TypeKind::TK_##name: \
      bid = AST::BuiltinType::BuiltinTypeId::BTI_##name; break;
  #include "Syntax/BuiltinTypes.def"
  }

  return m_ASTContext.GetBuiltinType(bid);
}

sona::ref_ptr<AST::DeclContext const>
SemaCommon::ChooseDeclContext(std::shared_ptr<Scope> scope,
                              std::vector<sona::strhdl_t> const& nns,
                              bool shouldDiag,
                              std::vector<SingleSourceRange> const& nnsRanges) {
  AST::QualType topLevelType = scope->LookupType(nns.front());
  if (topLevelType.GetUnqualTy() == nullptr) {
    if (shouldDiag) {
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrNotDeclared, {nns.front()}),
                  nnsRanges.front());
    }
    return nullptr;
  }

  if (topLevelType.GetUnqualTy()->GetTypeId()
      != AST::Type::TypeId::TI_UserDefined) {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrNotScope, {nns.front()}),
                nnsRanges.front());
    return nullptr;
  }

  sona::ref_ptr<AST::UserDefinedType const> udType =
      topLevelType.GetUnqualTy().cast_unsafe<AST::UserDefinedType const>();
  if (udType->GetUserDefinedTypeId()
      == AST::UserDefinedType::UDTypeId::UTI_Using) {
    m_Diag.Diag(Diag::DIR_Error,
                Diag::Format(Diag::DMT_ErrNotScope, {nns.front()}),
                nnsRanges.front());
    return nullptr;
  }

  sona::ref_ptr<AST::DeclContext const> context
      = udType->GetTypeDecl()->CastAsDeclContext();

  auto r1 = sona::linq::from_container(nns)
                        .zip_with(sona::linq::from_container(nnsRanges));

  for (auto it = r1.begin() + 1; it != r1.end(); ++it) {
    std::vector<sona::ref_ptr<AST::Decl const>> collectedDecls;
    context->LookupDeclContexts((*it).first, collectedDecls);
    if (collectedDecls.size() < 1) {
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrNotScope, {(*it).first}),
                  (*it).second);
      return nullptr;
    }
    if (collectedDecls.size() > 1) {
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrAmbiguousScope,
                               {(*it).first, (*(it - 1)).first}), (*it).second);
    }
    context = collectedDecls.front()->CastAsDeclContext();
  }

  return context;
}

AST::QualType
SemaCommon::LookupType(std::shared_ptr<Scope> scope,
                       const Syntax::Identifier& identifier, bool shouldDiag) {
  if (identifier.GetNestedNameSpecifiers().size() == 0) {
    AST::QualType ret = scope->LookupType(identifier.GetIdentifier());
    if (ret.GetUnqualTy() == nullptr && shouldDiag) {
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrNotDeclared,
                               { identifier.GetIdentifier() }),
                  identifier.GetIdSourceRange());
    }
    return ret;
  }

  sona::ref_ptr<AST::DeclContext const> context =
      ChooseDeclContext(scope, identifier.GetNestedNameSpecifiers(),
                        shouldDiag, identifier.GetNNSSourceRanges());
  if (context == nullptr) {
    return AST::QualType(nullptr);
  }

  std::vector<sona::ref_ptr<AST::Decl const>> collectedDecls;
  context->LookupTypeDecl(identifier.GetIdentifier(), collectedDecls);

  if (collectedDecls.size() < 1) {
    if (shouldDiag) {
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrNotDeclared,
                               { identifier.GetIdentifier() }),
                  identifier.GetIdSourceRange());
    }
    return AST::QualType(nullptr);
  }

  if (collectedDecls.size() > 1) {
    if (shouldDiag) {
      m_Diag.Diag(Diag::DIR_Error,
                  Diag::Format(Diag::DMT_ErrAmbiguous,
                               { identifier.GetIdentifier() }),
                  identifier.GetIdSourceRange());
    }
    return AST::QualType(nullptr);
  }

  return collectedDecls.front().cast_unsafe<AST::TypeDecl const>()
                       ->GetTypeForDecl();
}

} // namespace Sema
} // namespace ckx
