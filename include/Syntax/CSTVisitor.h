#ifndef CSTVISITOR_H
#define CSTVISITOR_H

#include <sona/result_wrapper.h>
#include <sona/pointer_plus.hpp>
#include <type_traits>

#include <Syntax/CST.h>

namespace ckx {
namespace Syntax {

using DeclResult = sona::result_wrapper;

class CSTDeclVisitor {
public:
  virtual DeclResult VisitClassDecl(sona::ref_ptr<CSTClassDecl> decl) = 0;
  virtual DeclResult VisitEnumDecl(sona::ref_ptr<CSTEnumDecl> decl) = 0;
  virtual DeclResult VisitFuncDecl(sona::ref_ptr<CSTFuncDecl> decl) = 0;
  virtual DeclResult VisitADTDecl(sona::ref_ptr<CSTADTDecl> decl) = 0;
};

} // namespace Syntax
} // namespace ckx

#endif // CSTVISITOR_H
