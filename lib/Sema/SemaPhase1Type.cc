#include "Sema/SemaPhase1.h"
#include "Syntax/Concrete.h"

namespace ckx {
namespace Sema {

AST::QualType
SemaPhase1::ResolveComposedType(std::shared_ptr<Scope> scope,
                                sona::ref_ptr<Syntax::ComposedType const> cty) {
  AST::QualType ret = ResolveType(scope, cty->GetRootType());

  /// @todo duplicate codes, remove them at some time.
  auto r = sona::linq::from_container(cty->GetTypeSpecifiers())
            .zip_with(
             sona::linq::from_container(cty->GetTypeSpecRanges()));
  for (const auto& p : r) {
    switch (p.first) {
    case Syntax::ComposedType::CTS_Pointer:
      ret = m_ASTContext.CreatePointerType(ret);
      break;
    case Syntax::ComposedType::CTS_Ref:
      ret = m_ASTContext.CreateLValueRefType(ret);
      break;
    case Syntax::ComposedType::CTS_RvRef:
      ret = m_ASTContext.CreateRValueRefType(ret);
      break;
    case Syntax::ComposedType::CTS_Const:
      if (ret.IsConst()) {
        m_Diag.Diag(Diag::DIR_Error,
                    Diag::Format(Diag::DMT_ErrDuplicateQual, {"const"}),
                    p.second);
      }
      else {
        ret.AddConst();
      }
      break;
    case Syntax::ComposedType::CTS_Volatile:
      if (ret.IsVolatile()) {
        m_Diag.Diag(Diag::DIR_Error,
                    Diag::Format(Diag::DMT_ErrDuplicateQual, {"volatile"}),
                    p.second);
      }
      else {
        ret.AddVolatile();
      }
      break;
    case Syntax::ComposedType::CTS_Restrict:
      if (ret.IsRestrict()) {
        m_Diag.Diag(Diag::DIR_Error,
                    Diag::Format(Diag::DMT_ErrDuplicateQual, {"restrict"}),
                    p.second);
      }
      else {
        ret.AddRestrict();
      }
      break;
    default:
      sona_unreachable1("not implemented");
    }
  }
  return ret;
}

AST::QualType
SemaPhase1::ResolveBuiltinType(std::shared_ptr<Scope>,
                               sona::ref_ptr<Syntax::BuiltinType const> bty) {
  return SemaCommon::ResolveBuiltinTypeImpl(bty);
}

AST::QualType
SemaPhase1::
ResolveUserDefinedType(std::shared_ptr<Scope> scope,
                       sona::ref_ptr<Syntax::UserDefinedType const> uty) {
  AST::QualType lookupResult = LookupType(scope, uty->GetName(), false);
  sona_assert(lookupResult.GetUnqualTy() != nullptr);
  return lookupResult;
}

AST::QualType
SemaPhase1::ResolveTemplatedType(std::shared_ptr<Scope>,
                                 sona::ref_ptr<Syntax::TemplatedType const>) {
  sona_unreachable1("not implemented");
  return sona::ref_ptr<AST::Type const>(nullptr);
}

sona::ref_ptr<AST::BuiltinType const>
SemaPhase1::CommonNumericType(sona::ref_ptr<AST::BuiltinType const> ty1,
                              sona::ref_ptr<AST::BuiltinType const> ty2) {
  if (ty1->IsSigned() && ty2->IsSigned()) {
    return std::max(ty1, ty2,
                    [this] (auto ty1, auto ty2) {
                      return this->SIntRank(ty1->GetBtid())
                             < this->SIntRank(ty2->GetBtid());
                    });
  }
  else if (ty1->IsUnsigned() && ty2->IsUnsigned()) {
    return std::max(ty1, ty2,
                    [this] (auto ty1, auto ty2) {
                      return this->UIntRank(ty1->GetBtid())
                             < this->UIntRank(ty2->GetBtid());
                    });
  }
  else if (ty1->IsFloating() && ty2->IsFloating()) {
    return std::max(ty1, ty2,
                    [this] (auto ty1, auto ty2) {
                      return this->FloatRank(ty1->GetBtid())
                             < this->FloatRank(ty2->GetBtid());
                    });
  }
  else {
    return nullptr;
  }
}

} // namespace Sema
} // namespace ckx
