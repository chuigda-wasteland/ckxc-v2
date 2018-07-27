#ifndef STMTBASE_HPP
#define STMTBASE_HPP

#include "StmtFwd.hpp"

namespace ckx {

class Stmt {
public:
  enum class StmtId {
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

protected:
  Stmt(StmtId id) : m_StmtId(id) {}

private:
  StmtId m_StmtId;
};

} // namespace ckx

#endif // STMTBASE_HPP
