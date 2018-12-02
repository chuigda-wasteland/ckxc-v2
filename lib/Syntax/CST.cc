#include "Syntax/CST.h"
#include <algorithm>

using namespace ckx;
using namespace Syntax;
using namespace std;
using namespace sona;

CSTNode::~CSTNode() {}

DeclResult CSTClassDecl::accept(CSTDeclVisitor &visitor) {
  return visitor.VisitClassDecl(this);
}

DeclResult CSTEnumDecl::accept(CSTDeclVisitor &visitor) {
  return visitor.VisitEnumDecl(this);
}

DeclResult CSTADTDecl::accept(CSTDeclVisitor &visitor) {
  return visitor.VisitADTDecl(this);
}

DeclResult CSTFuncDecl::accept(CSTDeclVisitor &visitor) {
  return visitor.VisitFuncDecl(this);
}
