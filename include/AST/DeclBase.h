#ifndef AST_DECLBASE_H
#define AST_DECLBASE_H

#include "AST/DeclFwd.h"
#include "Backend/ASTVisitor.h"

#include "sona/linq.h"
#include "sona/pointer_plus.h"
#include "sona/small_vector.h"
#include "sona/util.h"
#include "sona/stringref.h"
#include <vector>

namespace ckx {
namespace AST {

class Decl {
public:
  enum DeclKind : std::uint8_t {
    DK_Empty,
    DK_TransUnit,
    DK_Label,
    DK_Class,
    DK_Enum,
    DK_ADT,
    DK_ValueCtor,
    DK_Using,
    DK_Func,
    DK_Param,
    DK_Var,
    DK_Field,
    DK_Enumerator
  };

  enum DeclSpec {
    DS_None = 0x00,
    DS_Static = 0x01,
    DS_Internal = 0x02,
    DS_External = 0x04,
  };

  DeclKind GetDeclKind() const { return m_DeclKind; }

  sona::ref_ptr<DeclContext> CastAsDeclContext() noexcept;
  sona::ref_ptr<DeclContext const> CastAsDeclContext() const noexcept;

  virtual sona::owner<Backend::ActionResult> 
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

  void LookupDeclContexts(
         sona::strhdl_t const& name,
         std::vector<sona::ref_ptr<Decl const>> &recv) const;

  void LookupTypeDecl(sona::strhdl_t const& name,
                      std::vector<sona::ref_ptr<Decl const>> &recv) const;

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

#endif // AST_DECLBASE_H
