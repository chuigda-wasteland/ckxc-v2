#include "Syntax/CST.h"
#include <algorithm>

using namespace ckx;
using namespace Syntax;
using namespace std;
using namespace sona;

Node::~Node() {}

DeclResult ClassDecl::accept(CSTDeclVisitor &visitor) {
  return visitor.VisitClassDecl(this);
}

DeclResult EnumDecl::accept(CSTDeclVisitor &visitor) {
  return visitor.VisitEnumDecl(this);
}

DeclResult ADTDecl::accept(CSTDeclVisitor &visitor) {
  return visitor.VisitADTDecl(this);
}

DeclResult FuncDecl::accept(CSTDeclVisitor &visitor) {
  return visitor.VisitFuncDecl(this);
}
