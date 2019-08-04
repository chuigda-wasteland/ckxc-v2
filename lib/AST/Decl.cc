#include "AST/Decl.h"

namespace ckx {
namespace AST {

void DeclContext::LookupDeclContexts(
    const sona::strhdl_t &name,
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
    const sona::strhdl_t &name,
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

#define AST_DECL(name) \
sona::owner<Backend::ActionResult> \
name::Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const { \
  return visitor->Visit##name(this); \
}
#include "AST/Nodes.def"

} // namespace AST
} // namespace ckx
