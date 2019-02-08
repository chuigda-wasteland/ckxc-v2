#ifndef DECL_HPP
#define DECL_HPP

#include "AST/ASTContext.hpp"
#include "AST/DeclBase.hpp"
#include "AST/ExprBase.hpp"
#include "AST/TypeBase.hpp"

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
            sona::string_ref &&name)
    : Decl(declKind, declContext), m_Name(std::move(name)) {}

  sona::string_ref const &GetName() const noexcept { return m_Name; }

private:
  sona::string_ref m_Name;
};

class LabelDecl : public Decl {
public:
  LabelDecl(sona::ref_ptr<DeclContext> context,
            sona::string_ref const& labelString)
      : Decl(DeclKind::DK_Label, context), m_LabelString(labelString) {}

private:
  sona::string_ref m_LabelString;
};

class ClassDecl : public Decl, public DeclContext {
public:
  ClassDecl(sona::ref_ptr<DeclContext> context,
            sona::string_ref const& className)
      : Decl(DeclKind::DK_Class, context), DeclContext(DeclKind::DK_Class),
        m_ClassName(className) {}

  sona::string_ref const &GetName() const noexcept { return m_ClassName; }

private:
  sona::string_ref m_ClassName;
};

class EnumDecl : public Decl, public DeclContext {
public:
  EnumDecl(sona::ref_ptr<DeclContext> context,
           sona::string_ref const& enumName)
      : Decl(DeclKind::DK_Enum, context), DeclContext(DeclKind::DK_Enum),
        m_EnumName(enumName) {}

  sona::string_ref const &GetName() const { return m_EnumName; }

private:
  sona::string_ref m_EnumName;
};

class EnumeratorDecl : public Decl {
public:
  EnumeratorDecl(sona::ref_ptr<DeclContext> context,
                 sona::string_ref const& enumeratorName,
                 int64_t init)
      : Decl(DeclKind::DK_Enumerator, context),
        m_EnumeratorName(enumeratorName), m_Init(init) {}

  sona::string_ref const &GetEnumeratorName() const noexcept {
    return m_EnumeratorName;
  }

  int64_t GetInit() const noexcept {
    return m_Init;
  }

private:
  sona::string_ref m_EnumeratorName;
  int64_t m_Init;
};

class FuncDecl : public Decl, public DeclContext {
public:
  FuncDecl(sona::ref_ptr<DeclContext> context,
           sona::string_ref const& functionName,
           std::vector<sona::ref_ptr<Type const>> &&paramTypes,
           std::vector<sona::string_ref> &&paramNames,
           sona::ref_ptr<Type const> retType)
    : Decl(DeclKind::DK_Func, context), DeclContext(DeclKind::DK_Func),
      m_FunctionName(functionName),
      m_ParamTypes(std::move(paramTypes)),
      m_ParamNames(std::move(paramNames)),
      m_RetType(retType) {}

  sona::string_ref const& GetName() const noexcept {
    return m_FunctionName;
  }

  std::vector<sona::ref_ptr<Type const>> const&
  GetParamTypes() const noexcept {
    return m_ParamTypes;
  }

  std::vector<sona::string_ref> const& GetParamNames() const noexcept {
    return m_ParamNames;
  }

  sona::ref_ptr<Type const> GetRetType() const noexcept {
    return m_RetType;
  }

  bool IsDefinition() const noexcept {
    /// @todo provide a way to link declarations and definitions together.
    return false;
  }

private:
  sona::string_ref m_FunctionName;
  std::vector<sona::ref_ptr<Type const>> m_ParamTypes;
  std::vector<sona::string_ref> m_ParamNames;
  sona::ref_ptr<Type const> m_RetType;
};

class VarDecl : public Decl {
public:
  VarDecl(sona::ref_ptr<DeclContext> context,
          sona::ref_ptr<Type const> type,
          DeclSpec spec, sona::string_ref const& varName)
      : Decl(DeclKind::DK_Var, context), m_Type(type), m_DeclSpec(spec),
        m_VarName(varName) {}

  DeclSpec GetDeclSpec() const noexcept { return m_DeclSpec; }
  sona::string_ref const &GetVarName() const noexcept { return m_VarName; }

private:
  sona::ref_ptr<Type const> m_Type;
  DeclSpec m_DeclSpec;
  sona::string_ref m_VarName;
};

class ParamDecl : public VarDecl {};

class FieldDecl : public VarDecl {};

} // namespace AST
} // namespace ckx

#endif // DECL_HPP
