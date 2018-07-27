#ifndef DECL_HPP
#define DECL_HPP

#include "ASTContext.hpp"
#include "Basic/SourceRange.hpp"
#include "DeclBase.hpp"
#include "ExprBase.hpp"
#include "TypeBase.hpp"

#include <memory>
#include <string>

namespace ckx {

class TransUnitDecl : public Decl, public DeclContext {
public:
  TransUnitDecl()
      : Decl(DeclKind::DK_TransUnit, *this),
        DeclContext(DeclKind::DK_TransUnit) {}

  sona::ref_ptr<ASTContext> GetASTContext() noexcept { return m_Context; }

private:
  ASTContext m_Context;
};

class NamedDecl : public Decl {
public:
  NamedDecl(sona::ref_ptr<DeclContext> declContext, DeclKind declKind,
            std::string &&name, SourceRange &&nameRange)
      : Decl(declKind, declContext), m_Name(std::move(name)),
        m_NameRange(std::move(nameRange)) {}

  std::string const &GetName() const noexcept { return m_Name; }
  SourceRange const &GetNameRange() const noexcept { return m_NameRange; }

private:
  std::string m_Name;
  SourceRange m_NameRange;
};

class LabelDecl : public Decl {
public:
  LabelDecl(sona::ref_ptr<DeclContext> context, std::string &&labelString,
            SourceRange labelRange)
      : Decl(DeclKind::DK_Label, context),
        m_LabelString(std::move(labelString)), m_LabelRange(labelRange) {}

private:
  std::string m_LabelString;
  SourceRange m_LabelRange;
};

class ClassDecl : public Decl, public DeclContext {
public:
  ClassDecl(sona::ref_ptr<DeclContext> context, std::string &&className,
            SourceRange idRange, SourceLocation leftBraceLoc,
            SourceLocation rightBraceLoc)
      : Decl(DeclKind::DK_Class, context), DeclContext(DeclKind::DK_Class),
        m_ClassName(std::move(className)), m_IdRange(idRange),
        m_LeftBraceLoc(leftBraceLoc), m_RightBraceLoc(rightBraceLoc) {}

  std::string const &GetName() const { return m_ClassName; }
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
  EnumDecl(sona::ref_ptr<DeclContext> context, std::string &&enumName,
           SourceRange idRange, SourceLocation leftBraceLoc,
           SourceLocation rightBraceLoc)
      : Decl(DeclKind::DK_Enum, context), DeclContext(DeclKind::DK_Enum),
        m_EnumName(std::move(enumName)), m_IdRange(idRange),
        m_LeftBraceLoc(leftBraceLoc), m_RightBraceLoc(rightBraceLoc) {}

  std::string const &GetName() const { return m_EnumName; }
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
  EnumeratorDecl(sona::ref_ptr<DeclContext> context,
                 std::string &&enumeratorName, sona::owner<Expr> &&init,
                 SourceRange idRange, SourceLocation assignLocation)
      : Decl(DeclKind::DK_Enumerator, context),
        m_EnumeratorName(enumeratorName), m_Init(std::move(init)),
        m_IdRange(idRange), m_AssignLocation(assignLocation) {}

  std::string const &GetEnumeratorName() const { return m_EnumeratorName; }
  SourceRange GetIdSourceRange() const { return m_IdRange; }
  SourceLocation GetAssignLocation() const { return m_AssignLocation; }

private:
  std::string m_EnumeratorName;
  sona::owner<Expr> m_Init;
  SourceRange m_IdRange;
  SourceLocation m_AssignLocation;
};

class FunctionDecl : public Decl, public DeclContext {
public:
  FunctionDecl(sona::ref_ptr<DeclContext> context, std::string &&functionName,
               SourceLocation fnLocation, SourceRange idRange,
               SourceLocation leftParenLocation,
               SourceLocation rightParenLocation)
      : Decl(DeclKind::DK_Func, context), DeclContext(DeclKind::DK_Func),
        m_FunctionName(std::move(functionName)), m_FnLocation(fnLocation),
        m_IdRange(idRange), m_LeftParenLocation(leftParenLocation),
        m_RightParenLocation(rightParenLocation) {}

private:
  std::string m_FunctionName;
  SourceLocation m_FnLocation;
  SourceRange m_IdRange;
  SourceLocation m_LeftParenLocation, m_RightParenLocation;
};

class VarDecl : public Decl {
public:
  VarDecl(sona::ref_ptr<DeclContext> context, sona::ref_ptr<Type> type,
          DeclSpec spec, std::string &&varName, SourceRange varNameRange)
      : Decl(DeclKind::DK_Var, context), m_Type(type), m_DeclSpec(spec),
        m_VarName(std::move(varName)), m_VarNameRange(varNameRange) {}

  DeclSpec GetDeclSpec() const noexcept { return m_DeclSpec; }
  std::string const &GetVarName() const noexcept { return m_VarName; }
  SourceRange GetVarNameRange() const noexcept { return m_VarNameRange; }

private:
  sona::ref_ptr<Type> m_Type;
  DeclSpec m_DeclSpec;
  std::string m_VarName;
  SourceRange m_VarNameRange;
};

class ParamDecl : public Decl {};

class FieldDecl : public Decl {};

} // namespace ckx

#endif // DECL_HPP
