#ifndef STMT_HPP
#define STMT_HPP

#include "StmtBase.hpp"
#include "ExprFwd.hpp"
#include "DeclFwd.hpp"

#include "Basic/SourceRange.hpp"
#include "sona/pointer_plus.hpp"
#include "sona/optional.hpp"

namespace ckx {

class EmptyStmt : public Stmt {
public:
    EmptyStmt(SourceLocation location)
        : Stmt(StmtId::SI_Empty), m_Location(location) {}

    SourceLocation GetLocation() const noexcept { return m_Location; }

private:
    SourceLocation m_Location;
};

class DeclStmt : public Stmt {
public:
    DeclStmt(sona::owner<Decl> &&decl)
        : Stmt(StmtId::SI_Decl),
          m_Decl(std::move(decl)) {}

    sona::ref_ptr<Decl> GetDecl() const noexcept { return m_Decl.borrow(); }

private:
    sona::owner<Decl> m_Decl;
};

class ExprStmt : public Stmt {
public:
    ExprStmt(sona::owner<Expr> &&expr) :
        Stmt(StmtId::SI_Expr),
        m_Expr(std::move(expr)) {}

    sona::ref_ptr<Stmt> GetExpr() const noexcept { return m_Expr.borrow(); }

private:
    sona::owner<Expr> m_Expr;
};

/*
class CompoundStmt : public Stmt {
public:
    CompoundStmt(std::vector<Stmt> &&stmts,
                 SourceLocation leftBraceLocation,
                 SourceLocation rightBraceLocation)
        : Stmt(StmtId::SI_Compound),
          m_Stmts(std::move(stmts)),
          m_LeftBraceLocation(leftBraceLocation),
          m_RightBraceLocation(rightBraceLocation) {}

    void AddStmt(sona::ref_ptr<Stmt> stmt) { m_Stmts.push_back(stmt); }
    sona::ref_ptr<std::vector<Stmt> const> GetStmts() const {
        return sona::ref_ptr<std::vector<Stmt> const>(m_Stmts);
    }

    SourceLocation GetLeftBraceLocation() const noexcept {
        return m_LeftBraceLocation;
    }

    SourceLocation GetRightBraceLocation() const noexcept {
        return m_RightBraceLocation;
    }

private:
    std::vector<Stmt> m_Stmts;
    SourceLocation m_LeftBraceLocation,
                   m_RightBraceLocation;
};
*/
/// @todo On hold~

class IfStmt : public Stmt {
public:
    IfStmt(sona::owner<Expr> thenExpr,
           SourceLocation ifLocation,
           SourceLocation leftParenLocation,
           SourceLocation rightParenLocation)
        : Stmt(StmtId::SI_If),
          m_ThenExpr(std::move(thenExpr)),
          m_ElseExpr(sona::empty_optional()),
          m_IfLocation(ifLocation),
          m_LeftParenLocation(leftParenLocation),
          m_RightParenLocation(rightParenLocation),
          m_ElseLocation(sona::empty_optional()) {}

    IfStmt(Expr *thenExpr, Expr *elseExpr,
           SourceLocation ifLocation,
           SourceLocation leftParenLocation,
           SourceLocation rightParenLocation,
           SourceLocation elseLocation)
        : Stmt(StmtId::SI_If),
          m_ThenExpr(std::move(thenExpr)),
          m_ElseExpr(std::move(elseExpr)),
          m_IfLocation(ifLocation),
          m_LeftParenLocation(leftParenLocation),
          m_RightParenLocation(rightParenLocation),
          m_ElseLocation(elseLocation) {}

    sona::ref_ptr<Expr> GetThenExpr() const noexcept {
        return m_ThenExpr.borrow();
    }

    bool HasElse() const noexcept {
        return m_ElseExpr.has_value();
    }

    sona::ref_ptr<Expr> GetElseExpr() const noexcept {
        sona_assert(HasElse());
        return m_ElseExpr.value().borrow();
    }

    SourceLocation GetIfLocation() const noexcept {
        return m_IfLocation;
    }

    SourceLocation GetLeftParenLocation() const noexcept {
        return m_LeftParenLocation;
    }

    SourceLocation GetRightParenLocation() const noexcept {
        return m_RightParenLocation;
    }

    SourceLocation GetElseLocation() const noexcept {
        sona_assert(HasElse());
        return m_ElseLocation.value();
    }

private:
    sona::owner<Expr> m_ThenExpr;
    sona::optional<sona::owner<Expr>> m_ElseExpr;
    SourceLocation m_IfLocation;
    SourceLocation m_LeftParenLocation, m_RightParenLocation;
    sona::optional<SourceLocation> m_ElseLocation;
};

/// @todo support Match statement then

class ForStmt : public Stmt {
public:
    /*
    template <typename T1, typename T2, typename T3>
    ForStmt(T1 initExpr, T2 condExpr, T3 incrExpr, Stmt *stmt,
            SourceLocation forLocation,
            SourceLocation leftParenLocation,
            SourceLocation rightParenLocation,
            SourceLocation firstSemicolonLocation,
            SourceLocation secondSemicolonLocation)
        : m_InitExpr(std::move(initExpr)),
          m_CondExpr(std::move(condExpr)),
          m_IncrExpr(std::move(incrExpr)),
          m_Stmt(std::move(stmt)),
          m_ForLocation(forLocation),
          m_LeftParenLocation(leftParenLocation),
          m_RightParenLocation(rightParenLocation),
          m_FirstSemicolonLocation(firstSemicolonLocation),
          m_SecondSemicolonLocation(secondSemicolonLocation) {}
          */

    bool HasInitExpr() const noexcept { return m_InitExpr.has_value(); }
    bool HasCondExpr() const noexcept { return m_CondExpr.has_value(); }
    bool HasIncrExpr() const noexcept { return m_IncrExpr.has_value(); }

    sona::ref_ptr<Expr> GetInitExpr() const noexcept {
        sona_assert(HasInitExpr());
        return m_InitExpr.value().borrow();
    }

    sona::ref_ptr<Expr> GetCondExpr() const noexcept {
        sona_assert(HasCondExpr());
        return m_CondExpr.value().borrow();
    }

    sona::ref_ptr<Expr> GetIncrExpr() const noexcept {
        sona_assert(HasIncrExpr());
        return m_IncrExpr.value().borrow();
    }

    sona::ref_ptr<Stmt> GetStmt() const noexcept {
        return m_Stmt.borrow();
    }

    SourceLocation GetForLocation() const noexcept {
        return m_ForLocation;
    }

    SourceLocation GetLeftParenLocation() const noexcept {
        return m_LeftParenLocation;
    }

    SourceLocation GetRightParenLocation() const noexcept {
        return m_RightParenLocation;
    }

    SourceLocation GetFirstSemicolonLocation() const noexcept {
        return m_FirstSemicolonLocation;
    }

    SourceLocation GetSecondSemicolonLocation() const noexcept {
        return m_SecondSemicolonLocation;
    }

private:
    sona::optional<sona::owner<Expr>> m_InitExpr, m_CondExpr, m_IncrExpr;
    sona::owner<Stmt> m_Stmt;
    SourceLocation m_ForLocation;
    SourceLocation m_LeftParenLocation, m_RightParenLocation;
    SourceLocation m_FirstSemicolonLocation, m_SecondSemicolonLocation;
};

/// @todo ForIn

class WhileStmt : public Stmt {
public:

private:
    sona::owner<Expr> m_CondExpr;
    sona::owner<Stmt> m_Stmt;
    SourceLocation m_WhileLocation, m_LeftParenLocation, m_RightParenLocation;
};

class DoWhileStmt : public Stmt {
public:
private:
    sona::owner<Stmt> m_CondExpr;
    sona::owner<Stmt> m_Stmt;
    SourceLocation m_DoLocation, m_WhileLocation,
                   m_LeftParenLocation, m_RightParenLocation;
};

class BreakStmt : public Stmt {
private:
    SourceLocation m_Location;
}

class ContinueStmt : public Stmt {
public:
private:
    SourceLocation m_Location;
}

class ReturnStmt : public Stmt {
public:
private:
    SourceLocation m_ReturnLocation;
    sona::optional<sona::owner<Expr>> m_ReturnedExpr;
}

} // namespace ckx

#endif // STMT_HPP
