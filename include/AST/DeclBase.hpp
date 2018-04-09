#ifndef DECLBASE_HPP
#define DECLBASE_HPP

#include "DeclFwd.hpp"
#include <vector>
#include "sona/small_vector.hpp"
#include "sona/pointer_plus.hpp"
#include "sona/util.hpp"
#include "sona/linq.hpp"


namespace ckx {

class Decl {
public:
    enum DeclKind : std::uint8_t {
        DK_Label,
        DK_Class,
        DK_Field,
        DK_Enum,
        DK_Enumerator,
        DK_Function,
        DK_Param,
        DK_Var,
        DK_Module,
        DK_Import
    };

    DeclKind GetDeclKind() const { return m_DeclKind; }

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

    void AddDecl(sona::ref_ptr<Decl> decl);
    void RemoveDecl(sona::ref_ptr<Decl> decl);
    bool LookupDecl(sona::ref_ptr<Decl> decl);

private:
    Decl::DeclKind m_DeclKind;
    std::vector<sona::ref_ptr<Decl>> m_Decls;
};

} // namespace ckx

#endif // DECLBASE_HPP
