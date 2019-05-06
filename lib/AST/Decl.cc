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

} // namespace AST
} // namespace ckx
