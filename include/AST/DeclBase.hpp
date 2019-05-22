#ifndef DECLBASE_HPP
#define DECLBASE_HPP

#include "AST/DeclFwd.hpp"
#include "Backend/ASTVisitor.h"

#include "sona/linq.hpp"
#include "sona/pointer_plus.hpp"
#include "sona/small_vector.hpp"
#include "sona/util.hpp"
#include "sona/stringref.hpp"
#include <vector>

namespace ckx {
namespace AST {

enum class DeclSpec {
  DS_None = 0x00,
  DS_Static = 0x01,
  DS_Internal = 0x02,
  DS_External = 0x04,
};

class Decl {
public:
  enum DeclKind : std::uint8_t {
    DK_Empty,
    DK_TransUnit,
    DK_Label,
    DK_Class,
    DK_Enum,
    DK_EnumClass,
    DK_EnumClassIntern,
    DK_Using,
    DK_Func,
    DK_Param,
    DK_Var,
    DK_Field,
    DK_Enumerator
  };

  DeclKind GetDeclKind() const { return m_DeclKind; }

  sona::ref_ptr<DeclContext> CastAsDeclContext() noexcept;
  sona::ref_ptr<DeclContext const> CastAsDeclContext() const noexcept;

  virtual sona::owner<Backend::DeclResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const = 0;

  virtual ~Decl() noexcept = default;

protected:
  Decl(DeclKind declKind, sona::ref_ptr<DeclContext> context)
      : m_DeclKind(declKind), m_Context(context) {}

private:
  DeclKind m_DeclKind;
  sona::ref_ptr<DeclContext> m_Context;
};

class DeclContext {
public:
  DeclContext(Decl::DeclKind kind) : m_DeclKind(kind) {}

  void AddDecl(sona::owner<Decl> &&decl) {
    m_Decls.push_back(std::move(decl));
  }

  sona::ref_ptr<AST::Decl const>
  LookupDecl(sona::string_ref const& name) const;
  sona::ref_ptr<AST::Decl const>
  LookupDeclLocally(sona::string_ref const& name) const;

  auto GetDecls() const noexcept {
    (void)m_DeclKind;
    return sona::linq::from_container(m_Decls).
        transform([](sona::owner<Decl> const& decl) {
      return decl.borrow();
    });
  }

private:
  Decl::DeclKind m_DeclKind;
  std::vector<sona::owner<Decl>> m_Decls;
};

} // namespace AST
} // namespace ckx

#endif // DECLBASE_HPP
