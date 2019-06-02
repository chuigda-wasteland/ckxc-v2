#include "AST/Decl.h"

namespace ckx {
namespace AST {

void DeclContext::LookupDeclContexts(
    const sona::string_ref &name,
    std::vector<sona::ref_ptr<const Decl>> &recv) const {
  for (sona::ref_ptr<Decl const> decl : GetDecls()) {
    if ((decl->GetDeclKind() == AST::Decl::DK_TransUnit
         || decl->GetDeclKind() == AST::Decl::DK_Enum
         || decl->GetDeclKind() == AST::Decl::DK_ADT
         || decl->GetDeclKind() == AST::Decl::DK_Class)
        && decl.cast_unsafe<AST::NamedDecl const>()->GetName() == name) {
      recv.push_back(decl);
    }
  }
}

void DeclContext::LookupTypeDecl(
    const sona::string_ref &name,
    std::vector<sona::ref_ptr<const Decl> > &recv) const {
  for (sona::ref_ptr<Decl const> decl : GetDecls()) {
    if ((decl->GetDeclKind() == AST::Decl::DK_TransUnit
         || decl->GetDeclKind() == AST::Decl::DK_Enum
         || decl->GetDeclKind() == AST::Decl::DK_ADT
         || decl->GetDeclKind() == AST::Decl::DK_Class
         || decl->GetDeclKind() == AST::Decl::DK_Using)
        && decl.cast_unsafe<AST::NamedDecl const>()->GetName() == name) {
      recv.push_back(decl);
    }
  }
}

sona::ref_ptr<DeclContext> Decl::CastAsDeclContext() noexcept {
  switch (GetDeclKind()) {
  case DK_Enum:
    return static_cast<DeclContext*>(static_cast<EnumDecl*>(this));
  case DK_Class:
    return static_cast<DeclContext*>(static_cast<EnumDecl*>(this));
  case DK_ADT:
    return static_cast<DeclContext*>(static_cast<EnumDecl*>(this));
  default:
    sona_unreachable();
  }
  return nullptr;
}

sona::ref_ptr<DeclContext const> Decl::CastAsDeclContext() const noexcept {
  switch (GetDeclKind()) {
  case DK_Enum:
    return static_cast<DeclContext const*>(static_cast<EnumDecl const*>(this));
  case DK_Class:
    return static_cast<DeclContext const*>(static_cast<EnumDecl const*>(this));
  case DK_ADT:
    return static_cast<DeclContext const*>(static_cast<EnumDecl const*>(this));
  default:
    sona_unreachable();
  }
  return nullptr;
}

sona::owner<Backend::ActionResult> 
TransUnitDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitTransUnit(this);
}

sona::owner<Backend::ActionResult>
LabelDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitLabelDecl(this);
}

sona::owner<Backend::ActionResult>
ClassDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitClassDecl(this);
}

sona::owner<Backend::ActionResult>
EnumDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitEnumDecl(this);
}

sona::owner<Backend::ActionResult>
ValueCtorDecl::Accept(
    sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitValueCtor(this);
}

sona::owner<Backend::ActionResult>
ADTDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitADT(this);
}

sona::owner<Backend::ActionResult>
EnumeratorDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitEnumeratorDecl(this);
}

sona::owner<Backend::ActionResult>
UsingDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitUsingDecl(this);
}

sona::owner<Backend::ActionResult>
FuncDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitFuncDecl(this);
}

sona::owner<Backend::ActionResult>
VarDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitVarDecl(this);
}

} // namespace AST
} // namespace ckx
