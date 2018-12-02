#ifndef CSTFWD_H
#define CSTFWD_H

namespace ckx {
namespace Syntax {

class CSTNode;
  class CSTAttributeList;
  class CSTIdentifier;
  class CSTImport;
  class CSTExport;

  class CSTType;
    class CSTBasicType;
    class CSTUserDefinedType;
    class CSTTemplatedType;
    class CSTComposedType;

  class CSTDecl;
    class CSTTemplatedDecl;
    class CSTForwardDecl;
    class CSTClassDecl;
    class CSTEnumDecl;
    class CSTADTDecl;
    class CSTFuncDecl;
    class CSTVarDecl;

  class CSTStmt;
    class CSTEmptyStmt;
    class CSTExprStmt;
    class CSTIfStmt;
    class CSTMatchStmt;
    class CSTForStmt;
    class CSTForEachStmt;
    class CSTWhileStmt;
    class CSTCompoundStmt;
    class CSTReturnStmt;

  class CSTExpr;
    class CSTLiteralExpr;
    class CSTStringLiteralExpr;
    class CSTIdRefExpr;
    class CSTFuncCallExpr;
    class CSTUnaryExpr;
    class CSTBinaryExpr;
    class CSTMixFixExpr;

} // namespace Syntax
} // namespace ckx

#endif // CSTFWD_H
