#include "AST/Decl.hpp"

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

sona::owner<Backend::DeclResult> 
TransUnitDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) {
  return visitor->VisitTransUnit(this);
}

sona::owner<Backend::DeclResult>
LabelDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) {
  return visitor->VisitLabelDecl(this);
}

sona::owner<Backend::DeclResult>
ClassDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) {
  return visitor->VisitClassDecl(this);
}

sona::owner<Backend::DeclResult>
EnumDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) {
  return visitor->VisitEnumDecl(this);
}

sona::owner<Backend::DeclResult>
EnumClassInternDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) {
  return visitor->VisitEnumClassInternDecl(this);
}

sona::owner<Backend::DeclResult>
EnumClassDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) {
  return visitor->VisitEnumClassDecl(this);
}

sona::owner<Backend::DeclResult>
EnumeratorDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) {
  return visitor->VisitEnumeratorDecl(this);
}

sona::owner<Backend::DeclResult>
UsingDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) {
  return visitor->VisitUsingDecl(this);
}

sona::owner<Backend::DeclResult>
FuncDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) {
  return visitor->VisitFuncDecl(this);
}

sona::owner<Backend::DeclResult>
VarDecl::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) {
  return visitor->VisitVarDecl(this);
}

} // namespace AST
} // namespace ckx
