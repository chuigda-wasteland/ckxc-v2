#include "AST/Decl.h"

namespace ckx {
namespace AST {

sona::ref_ptr<Decl const>
DeclContext::LookupDecl(sona::string_ref const& name) const {
  (void)name;
  sona_unreachable();
  return nullptr;
}

sona::ref_ptr<Decl const>
DeclContext::LookupDeclLocally(sona::string_ref const& name) const {
  (void)name;
  sona_unreachable();
  return nullptr;
}

sona::ref_ptr<DeclContext> Decl::CastAsDeclContext() noexcept {
  switch (GetDeclKind()) {
  case DK_Enum:
    return static_cast<DeclContext*>(static_cast<EnumDecl*>(this));
  case DK_Class:
    return static_cast<DeclContext*>(static_cast<EnumDecl*>(this));
  case DK_EnumClass:
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
  case DK_EnumClass:
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
EnumClassInternDecl::Accept(
    sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitEnumClassInternDecl(this);
}

sona::owner<Backend::ActionResult>
EnumClassDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const {
  return visitor->VisitEnumClassDecl(this);
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
