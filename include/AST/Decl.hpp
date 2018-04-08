#ifndef DECL_HPP
#define DECL_HPP

#include "AST/DeclBase.hpp"
#include "Basic/SourceRange.hpp"

#include <string>
#include <memory>

namespace ckx {

using sona::ref_ptr;

class LabelDecl : public Decl {
public:
    LabelDecl(ref_ptr<DeclContext> context,
              std::string &&labelString,
              SourceRange labelRange)
        : Decl(DeclKind::DK_Label, context),
          m_LabelString(std::move(labelString)),
          m_LabelRange(labelRange) {}

private:
    std::string m_LabelString;
    SourceRange m_LabelRange;
};

class ClassDecl : public Decl, public DeclContext {
public:
    ClassDecl(ref_ptr<DeclContext> context,
              std::string &&className,
              SourceRange idRange,
              SourceLocation leftBraceLoc,
              SourceLocation rightBraceLoc)
        : Decl(DeclKind::DK_Class, context),
          DeclContext(DeclKind::DK_Class),
          m_ClassName(std::move(className)),
          m_IdRange(idRange),
          m_LeftBraceLoc(leftBraceLoc),
          m_RightBraceLoc(rightBraceLoc) {}

    std::string const& GetName() const { return m_ClassName; }
    SourceRange GetIdSourceRange() const { return m_IdRange; }
    SourceLocation GetLeftBraceLocation() const { return m_LeftBraceLoc; }
    SourceLocation GetRightBraceLocation() const { return m_RightBraceLoc; }

private:
    std::string m_ClassName;
    SourceRange m_IdRange;
    SourceLocation m_LeftBraceLoc, m_RightBraceLoc;
};

class EnumDecl : public Decl, public DeclContext {
public:
    EnumDecl(ref_ptr<DeclContext> context,
             std::string &&enumName,
             SourceRange idRange,
             SourceLocation leftBraceLoc,
             SourceLocation rightBraceLoc)
        : Decl(DeclKind::DK_Enum, context),
          DeclContext(DeclKind::DK_Enum),
          m_EnumName(std::move(enumName)),
          m_IdRange(idRange),
          m_LeftBraceLoc(leftBraceLoc),
          m_RightBraceLoc(rightBraceLoc) {}

    std::string const& GetName() const { return m_EnumName; }
    SourceRange GetIdSourceRange() const { return m_IdRange; }
    SourceLocation GetLeftBraceLocation() const { return m_LeftBraceLoc; }
    SourceLocation GetRightBraceLocation() const { return m_RightBraceLoc; }

private:
    std::string m_EnumName;
    SourceRange m_IdRange;
    SourceLocation m_LeftBraceLoc, m_RightBraceLoc;
};

class EnumeratorDecl : public Decl {
public:
    EnumeratorDecl(ref_ptr<DeclContext> context,
                   std::string &&enumeratorName,
                   SourceRange idRange,
                   SourceLocation assignLocation
                   /** @todo ref_ptr<Expr> initializer */) :
        Decl(DeclKind::DK_Enumerator, context),
        m_EnumeratorName(enumeratorName),
        m_IdRange(idRange),
        m_AssignLocation(assignLocation) {}

    std::string const& GetEnumeratorName() const { return m_EnumeratorName; }
    SourceRange GetIdSourceRange() const { return m_IdRange; }
    SourceLocation GetAssignLocation() const { return m_AssignLocation; }

private:
    std::string m_EnumeratorName;
    SourceRange m_IdRange;
    SourceLocation m_AssignLocation;
};

class FunctionDecl : public Decl, public DeclContext {
public:
    FunctionDecl(ref_ptr<DeclContext> context,
                 std::string &&functionName,
                 SourceLocation fnLocation,
                 SourceRange idRange,
                 SourceLocation leftParenLocation,
                 SourceLocation rightParenLocation)
        : Decl(DeclKind::DK_Function, context),
          DeclContext(DeclKind::DK_Function),
          m_FunctionName(std::move(functionName)),
          m_FnLocation(fnLocation),
          m_IdRange(idRange),
          m_LeftParenLocation(leftParenLocation),
          m_RightParenLocation(rightParenLocation) {}

private:
    std::string m_FunctionName;
    SourceLocation m_FnLocation;
    SourceRange m_IdRange;
    SourceLocation m_LeftParenLocation,
                   m_RightParenLocation;
};

} // namespace ckx

#endif // DECL_HPP
