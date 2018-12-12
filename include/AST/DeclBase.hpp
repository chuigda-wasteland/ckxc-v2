#ifndef DECLBASE_HPP
#define DECLBASE_HPP

#include "DeclFwd.hpp"
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

  void AddDecl(sona::owner<Decl> &&decl);
  bool LookupDecl(sona::string_ref name);

private:
  Decl::DeclKind m_DeclKind;
  std::vector<sona::owner<Decl>> m_Decls;
};

} // namespace AST
} // namespace ckx

#endif // DECLBASE_HPP
