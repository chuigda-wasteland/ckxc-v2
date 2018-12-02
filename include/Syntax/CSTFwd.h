#ifndef CSTFWD_H
#define CSTFWD_H

namespace ckx {
namespace Syntax {

class CSTNode;
  class CSTIdentifier;
  class CSTImport;
  class CSTExport;
  class CSTType;
    class CSTBasicType;
    class CSTUserDefinedType;
    class CSTTemplatedType;
    class CSTComposedType;
  class CSTDecl;
    class CSTClassDecl;
    class CSTEnumDecl;
    class CSTADTDecl;
    class CSTFuncDecl;
  class CSTStmt;
  class CSTExpr;

}
}

#endif // CSTFWD_H
