#ifndef DECL_HPP
#define DECL_HPP

#include "ASTContext.hpp"
#include "Basic/SourceRange.hpp"
#include "DeclBase.hpp"
#include "ExprBase.hpp"
#include "TypeBase.hpp"

#include "Expr.hpp"

#include <memory>
#include <string>

namespace ckx {
namespace AST {

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
            std::string &&name)
    : Decl(declKind, declContext), m_Name(std::move(name)) {}

  std::string const &GetName() const noexcept { return m_Name; }

private:
  std::string m_Name;
};

class LabelDecl : public Decl {
public:
  LabelDecl(sona::ref_ptr<DeclContext> context, std::string &&labelString)
      : Decl(DeclKind::DK_Label, context),
        m_LabelString(std::move(labelString)) {}

private:
  std::string m_LabelString;
};

class ClassDecl : public Decl, public DeclContext {
public:
  ClassDecl(sona::ref_ptr<DeclContext> context, std::string &&className)
      : Decl(DeclKind::DK_Class, context), DeclContext(DeclKind::DK_Class),
        m_ClassName(std::move(className)) {}

  std::string const &GetName() const { return m_ClassName; }

private:
  std::string m_ClassName;
};

class EnumDecl : public Decl, public DeclContext {
public:
  EnumDecl(sona::ref_ptr<DeclContext> context, std::string &&enumName)
      : Decl(DeclKind::DK_Enum, context), DeclContext(DeclKind::DK_Enum),
        m_EnumName(std::move(enumName)) {}
  std::string const &GetName() const { return m_EnumName; }

private:
  std::string m_EnumName;
};

class EnumeratorDecl : public Decl {
public:
  EnumeratorDecl(sona::ref_ptr<DeclContext> context,
                 std::string &&enumeratorName, sona::owner<Expr> &&init)
      : Decl(DeclKind::DK_Enumerator, context),
        m_EnumeratorName(enumeratorName), m_Init(std::move(init)) {}

  std::string const &GetEnumeratorName() const { return m_EnumeratorName; }

private:
  std::string m_EnumeratorName;
  sona::owner<Expr> m_Init;
};

class FunctionDecl : public Decl, public DeclContext {
public:
  FunctionDecl(sona::ref_ptr<DeclContext> context, std::string &&functionName)
    : Decl(DeclKind::DK_Func, context), DeclContext(DeclKind::DK_Func),
      m_FunctionName(std::move(functionName)) {}

private:
  std::string m_FunctionName;
};

class VarDecl : public Decl {
public:
  VarDecl(sona::ref_ptr<DeclContext> context, sona::ref_ptr<Type> type,
          DeclSpec spec, std::string &&varName)
      : Decl(DeclKind::DK_Var, context), m_Type(type), m_DeclSpec(spec),
        m_VarName(std::move(varName)) {}

  DeclSpec GetDeclSpec() const noexcept { return m_DeclSpec; }
  std::string const &GetVarName() const noexcept { return m_VarName; }

private:
  sona::ref_ptr<Type> m_Type;
  DeclSpec m_DeclSpec;
  std::string m_VarName;
};

class ParamDecl : public VarDecl {};

class FieldDecl : public VarDecl {};

} // namespace AST
} // namespace ckx

#endif // DECL_HPP
