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
  case AST::Decl::DK_EnumClass: {
    sona::ref_ptr<AST::EnumClassDecl const> enumClassDecl =
        GetHalfway().cast_unsafe<AST::EnumClassDecl const>();
    ret += enumClassDecl->GetName().get();
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

std::string IncompleteEnumClassInternDecl::ToString() const {
  return "incomplete ADT constructor: "
         + GetHalfway().cast_unsafe<AST::EnumClassInternDecl const>()
                       ->GetConstructorName().get();
}

std::string IncompleteFuncDecl::ToString() const {
  return "incomplete function: " + m_FuncDecl->GetName().get();
}

} // namespace Sema
} // namespace ckx
