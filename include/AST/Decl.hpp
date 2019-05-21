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

class TransUnitDecl final : public Decl, public DeclContext {
public:
  TransUnitDecl()
    : Decl(DeclKind::DK_TransUnit, *this),
      DeclContext(DeclKind::DK_TransUnit) {}

  sona::ref_ptr<ASTContext> GetASTContext() noexcept { return m_Context; }

  sona::owner<Backend::DeclResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) override;

private:
  ASTContext m_Context;
};

class NamedDecl : public Decl {
public:
  NamedDecl(sona::ref_ptr<DeclContext> declContext, DeclKind declKind,
            sona::string_ref const& name)
    : Decl(declKind, declContext), m_Name(name) {}

  sona::string_ref const &GetName() const noexcept { return m_Name; }

private:
  sona::string_ref m_Name;
};

class TypeDecl : public NamedDecl {
public:
  TypeDecl(sona::ref_ptr<DeclContext> declContext, DeclKind declKind,
           sona::string_ref const& name)
    : NamedDecl(declContext, declKind, name), m_TypeForDecl(nullptr) {}

  sona::ref_ptr<AST::Type> GetTypeForDecl() noexcept {
    return m_TypeForDecl;
  }

  sona::ref_ptr<AST::Type const> GetTypeForDecl() const noexcept {
    return m_TypeForDecl;
  }

  void SetTypeForDecl(sona::ref_ptr<AST::Type> typeForDecl) noexcept {
    m_TypeForDecl = typeForDecl;
  }

private:
  sona::ref_ptr<AST::Type> m_TypeForDecl;
};

class LabelDecl final : public Decl {
public:
  LabelDecl(sona::ref_ptr<DeclContext> context,
            sona::string_ref const& labelString)
      : Decl(DeclKind::DK_Label, context), m_LabelString(labelString) {}

  sona::owner<Backend::DeclResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) override;

private:
  sona::string_ref m_LabelString;
};

class ClassDecl final : public TypeDecl, public DeclContext {
public:
  ClassDecl(sona::ref_ptr<DeclContext> context,
            sona::string_ref const& className)
      : TypeDecl(context, DeclKind::DK_Class, className),
        DeclContext(DeclKind::DK_Class) {}
        
  sona::owner<Backend::DeclResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) override;
};

class EnumDecl final : public TypeDecl, public DeclContext {
public:
  EnumDecl(sona::ref_ptr<DeclContext> context,
           sona::string_ref const& enumName)
      : TypeDecl(context, DeclKind::DK_Enum, enumName),
        DeclContext(DeclKind::DK_Enum) {}
        
  sona::owner<Backend::DeclResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) override;
};

class EnumClassInternDecl final : public Decl {
public:
  EnumClassInternDecl(sona::ref_ptr<DeclContext> context,
                      sona::string_ref const& constructorName,
                      sona::ref_ptr<AST::Type const> type)
    : Decl(DeclKind::DK_EnumClassIntern, context),
      m_ConstructorName(constructorName),
      m_Type(type) {}

  sona::string_ref const& GetConstructorName() const noexcept {
    return m_ConstructorName;
  }

  sona::ref_ptr<AST::Type const> GetType() const noexcept {
    return m_Type;
  }

  void SetType(sona::ref_ptr<AST::Type const> type) noexcept {
    m_Type = type;
  }

  sona::owner<Backend::DeclResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) override;

private:
  sona::string_ref m_ConstructorName;
  sona::ref_ptr<AST::Type const> m_Type;
};

class EnumClassDecl final : public TypeDecl, public DeclContext {
public:
  EnumClassDecl(sona::ref_ptr<DeclContext> context,
                sona::string_ref const& enumClassName)
    : TypeDecl(context, DeclKind::DK_EnumClass, enumClassName),
      DeclContext(DeclKind::DK_EnumClass) {}
      
  sona::owner<Backend::DeclResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) override;
};

class UsingDecl final : public TypeDecl {
public:
  UsingDecl(sona::ref_ptr<DeclContext> context,
            sona::string_ref const& aliasName,
            sona::ref_ptr<AST::Type const> aliasee)
    : TypeDecl(context, DeclKind::DK_Using, aliasName), m_Aliasee(aliasee) {}

  /// @note only use this for refilling after dependency resolution
  void FillAliasee(sona::ref_ptr<AST::Type const> aliasee) noexcept {
    sona_assert(m_Aliasee == nullptr);
    m_Aliasee = aliasee;
  }

  sona::ref_ptr<AST::Type const> GetAliasee() const noexcept {
    return m_Aliasee;
  }

  sona::owner<Backend::DeclResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) override;

private:
  sona::ref_ptr<AST::Type const> m_Aliasee;
};

class EnumeratorDecl final : public Decl {
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
  
  sona::owner<Backend::DeclResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) override;

private:
  sona::string_ref m_EnumeratorName;
  int64_t m_Init;
};

class FuncDecl final : public Decl, public DeclContext {
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
  
  sona::owner<Backend::DeclResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) override;

private:
  sona::string_ref m_FunctionName;
  std::vector<sona::ref_ptr<Type const>> m_ParamTypes;
  std::vector<sona::string_ref> m_ParamNames;
  sona::ref_ptr<Type const> m_RetType;
};

class VarDecl final : public Decl {
public:
  VarDecl(sona::ref_ptr<DeclContext> context,
          sona::ref_ptr<Type const> type,
          DeclSpec spec, sona::string_ref const& varName)
      : Decl(DeclKind::DK_Var, context), m_Type(type), m_DeclSpec(spec),
        m_VarName(varName) {}

  DeclSpec GetDeclSpec() const noexcept { return m_DeclSpec; }
  sona::string_ref const &GetVarName() const noexcept { return m_VarName; }
  void SetType(sona::ref_ptr<Type const> type) noexcept { m_Type = type; }
  sona::ref_ptr<Type const> GetType() const noexcept { return m_Type; }

  sona::owner<Backend::DeclResult>
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) override;

private:
  sona::ref_ptr<Type const> m_Type;
  DeclSpec m_DeclSpec;
  sona::string_ref m_VarName;
};

// class ParamDecl : public VarDecl {};

// class FieldDecl : public VarDecl {};

} // namespace AST
} // namespace ckx

#endif // DECL_HPP
