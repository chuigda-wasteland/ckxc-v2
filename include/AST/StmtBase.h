#ifndef AST_STMTBASE_H
#define AST_STMTBASE_H

#include "StmtFwd.h"
#include "Backend/ASTVisitor.h"

#include "sona/stringref.h"

namespace ckx {
namespace AST {

class Stmt {
public:
  enum StmtId {
    SI_Empty,
    SI_Decl,
    SI_Expr,
    SI_Compound,
    SI_If,
    SI_Match,
    SI_For,
    SI_ForIn,
    SI_While,
    SI_DoWhile,
    SI_Break,
    SI_Continue,
    SI_Return
  };

  StmtId GetStmtId() const { return m_StmtId; }

  virtual ~Stmt() noexcept = default;

  virtual sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::StmtVisitor> visitor) const = 0;

protected:
  Stmt(StmtId id) : m_StmtId(id) {}

private:
  StmtId m_StmtId;
};

} // namespace AST
} // namespace ckx

#endif // AST_STMTBASE_H
