#include "Sema/UnresolvedDecl.h"
#include "Sema/Scope.h"
#include "AST/Type.h"

namespace ckx {
namespace Sema {

void IncompleteDecl::AddNameDepend(Syntax::Identifier&& id, bool isStrong) {
  m_Dependencies.emplace_back(std::move(id), isStrong);
}

void IncompleteDecl::AddValueDepend(sona::ref_ptr<AST::Decl> decl,
                                    bool isStrong) {
  m_Dependencies.emplace_back(decl, isStrong);
}

std::string IncompleteVarDecl::ToString() const {
  return "incomplete variable: " + GetConcrete()->GetName().get();
}

std::string IncompleteTagDecl::ToString() const {
  std::string ret = "incomplete tag: ";
  switch (GetHalfway()->GetDeclKind()) {
  case AST::Decl::DK_Class: {
    sona::ref_ptr<AST::ClassDecl const> classDecl =
        GetHalfway().cast_unsafe<AST::ClassDecl const>();
    ret += classDecl->GetName().get();
    break;
  }
  case AST::Decl::DK_Enum: {
    sona::ref_ptr<AST::EnumDecl const> enumDecl =
        GetHalfway().cast_unsafe<AST::EnumDecl const>();
    ret += enumDecl->GetName().get();
    break;
  }
  case AST::Decl::DK_ADT: {
    sona::ref_ptr<AST::ADTDecl const> adtDecl =
        GetHalfway().cast_unsafe<AST::ADTDecl const>();
    ret += adtDecl->GetName().get();
    break;
  }
  default:
    sona_unreachable();
  }
  return ret;
}

std::string IncompleteUsingDecl::ToString() const {
  return "incomplete using: " + GetHalfway()->GetName().get();
}

std::string IncompleteValueCtorDecl::ToString() const {
  return "incomplete ADT constructor: "
         + GetHalfway().cast_unsafe<AST::ValueCtorDecl const>()
                       ->GetConstructorName().get();
}

std::string IncompleteFuncDecl::ToString() const {
  return "incomplete function: " + m_FuncDecl->GetName().get();
}

} // namespace Sema
} // namespace ckx
