#ifndef STMT_HPP
#define STMT_HPP

#include "DeclBase.hpp"
#include "ExprBase.hpp"
#include "StmtBase.hpp"

#include "sona/optional.hpp"
#include "sona/pointer_plus.hpp"

namespace ckx {
namespace AST {

class EmptyStmt : public Stmt {
public:
  EmptyStmt() : Stmt(StmtId::SI_Empty) {}
};

class DeclStmt : public Stmt {
public:
  DeclStmt(sona::owner<Decl> &&decl)
    : Stmt(StmtId::SI_Decl), m_Decl(std::move(decl)) {}

  sona::ref_ptr<Decl const> GetDecl() const noexcept { return m_Decl.borrow(); }

private:
  sona::owner<Decl> m_Decl;
};

class ExprStmt : public Stmt {
public:
  ExprStmt(sona::owner<Expr> &&expr)
      : Stmt(StmtId::SI_Expr), m_Expr(std::move(expr)) {}

  sona::ref_ptr<Expr const> GetExpr() const noexcept { return m_Expr.borrow(); }

private:
  sona::owner<Expr> m_Expr;
};

class CompoundStmt : public Stmt {
public:
  CompoundStmt(std::vector<Stmt> &&stmts)
        : Stmt(StmtId::SI_Compound),
          m_Stmts(std::move(stmts)) {}

    void AddStmt(sona::ref_ptr<Stmt> stmt) { m_Stmts.push_back(stmt); }

    sona::ref_ptr<std::vector<Stmt> const> GetStmts() const {
        return sona::ref_ptr<std::vector<Stmt> const>(m_Stmts);
    }

private:
    std::vector<Stmt> m_Stmts;
};

/// @todo On hold~

class IfStmt : public Stmt {
public:
  IfStmt(sona::owner<Expr> thenExpr)
    : Stmt(StmtId::SI_If), m_ThenExpr(std::move(thenExpr)),
      m_ElseExpr(sona::empty_optional()) {}
  IfStmt(sona::owner<Expr> thenExpr, sona::owner<Expr> elseExpr)
    : Stmt(StmtId::SI_If), m_ThenExpr(std::move(thenExpr)),
      m_ElseExpr(std::move(elseExpr)) {}

  sona::ref_ptr<Expr const> GetThenExpr() const noexcept {
    return m_ThenExpr.borrow();
  }

  bool HasElse() const noexcept { return m_ElseExpr.has_value(); }

  sona::ref_ptr<Expr const> GetElseExprUnsafe() const noexcept {
    sona_assert(HasElse());
    return m_ElseExpr.value().borrow();
  }

private:
  sona::owner<Expr> m_ThenExpr;
  sona::optional<sona::owner<Expr>> m_ElseExpr;
};

/// @todo support Match statement then

class ForStmt : public Stmt {
public:
  template <typename T1, typename T2, typename T3>
  ForStmt(T1 initExpr, T2 condExpr, T3 incrExpr, sona::owner<Stmt> &&stmt)
      : m_InitExpr(std::move(initExpr)), m_CondExpr(std::move(condExpr)),
        m_IncrExpr(std::move(incrExpr)), m_Stmt(std::move(stmt)) {
    static_assert(std::is_same<T1, sona::owner<Expr>>::value ||
                      std::is_same<T1, sona::empty_optional>::value,
                  "");
    static_assert(std::is_same<T2, sona::owner<Expr>>::value ||
                      std::is_same<T2, sona::empty_optional>::value,
                  "");
    static_assert(std::is_same<T3, sona::owner<Expr>>::value ||
                      std::is_same<T3, sona::empty_optional>::value,
                  "");
  }

  bool HasInitExpr() const noexcept { return m_InitExpr.has_value(); }
  bool HasCondExpr() const noexcept { return m_CondExpr.has_value(); }
  bool HasIncrExpr() const noexcept { return m_IncrExpr.has_value(); }

  sona::ref_ptr<Expr const> GetInitExprUnsafe() const noexcept {
    sona_assert(HasInitExpr());
    return m_InitExpr.value().borrow();
  }

  sona::ref_ptr<Expr const> GetCondExprUnsafe() const noexcept {
    sona_assert(HasCondExpr());
    return m_CondExpr.value().borrow();
  }

  sona::ref_ptr<Expr const> GetIncrExprUnsafe() const noexcept {
    sona_assert(HasIncrExpr());
    return m_IncrExpr.value().borrow();
  }

  sona::ref_ptr<Stmt const> GetStmt() const noexcept { return m_Stmt.borrow(); }

private:
  sona::optional<sona::owner<Expr>> m_InitExpr, m_CondExpr, m_IncrExpr;
  sona::owner<Stmt> m_Stmt;
};

/// @todo ForIn

class WhileStmt : public Stmt {
public:
private:
  sona::owner<Expr> m_CondExpr;
  sona::owner<Stmt> m_Stmt;
};

class DoWhileStmt : public Stmt {
public:
private:
  sona::owner<Stmt> m_CondExpr;
  sona::owner<Stmt> m_Stmt;
};

class BreakStmt : public Stmt {
private:
};

class ContinueStmt : public Stmt {
public:
private:
};

class ReturnStmt : public Stmt {
public:
private:
  sona::optional<sona::owner<Expr>> m_ReturnedExpr;
};

} // namespace AST
} // namespace ckx

#endif // STMT_HPP
