#ifndef AST_DECL_H
#define AST_DECL_H

#include "AST/ASTContext.h"
#include "AST/DeclBase.h"
#include "AST/ExprBase.h"
#include "AST/TypeBase.h"

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

  sona::owner<Backend::ActionResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const override;

private:
  ASTContext m_Context;
};

class NamedDecl : public Decl {
public:
  NamedDecl(sona::ref_ptr<DeclContext> declContext, DeclKind declKind,
            sona::strhdl_t const& name)
    : Decl(declKind, declContext), m_Name(name) {}

  sona::strhdl_t const &GetName() const noexcept { return m_Name; }

private:
  sona::strhdl_t m_Name;
};

class TypeDecl : public NamedDecl {
public:
  TypeDecl(sona::ref_ptr<DeclContext> declContext, DeclKind declKind,
           sona::strhdl_t const& name)
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
            sona::strhdl_t const& labelString)
      : Decl(DeclKind::DK_Label, context), m_LabelString(labelString) {}

  sona::owner<Backend::ActionResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const override;

  sona::strhdl_t const& GetLabelString() const noexcept {
    return m_LabelString;
  }

private:
  sona::strhdl_t m_LabelString;
};

class ClassDecl final : public TypeDecl, public DeclContext {
public:
  ClassDecl(sona::ref_ptr<DeclContext> context,
            sona::strhdl_t const& className)
      : TypeDecl(context, DeclKind::DK_Class, className),
        DeclContext(DeclKind::DK_Class) {}
        
  sona::owner<Backend::ActionResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const override;
};

class EnumDecl final : public TypeDecl, public DeclContext {
public:
  EnumDecl(sona::ref_ptr<DeclContext> context,
           sona::strhdl_t const& enumName)
      : TypeDecl(context, DeclKind::DK_Enum, enumName),
        DeclContext(DeclKind::DK_Enum) {}
        
  sona::owner<Backend::ActionResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const override;
};

class ValueCtorDecl final : public Decl {
public:
  ValueCtorDecl(sona::ref_ptr<DeclContext> context,
                sona::strhdl_t const& constructorName,
                QualType type)
    : Decl(DeclKind::DK_ValueCtor, context),
      m_ConstructorName(constructorName),
      m_Type(type) {}

  sona::strhdl_t const& GetConstructorName() const noexcept {
    return m_ConstructorName;
  }

  QualType GetType() const noexcept {
    return m_Type;
  }

  void SetType(QualType type) noexcept {
    m_Type = type;
  }

  sona::owner<Backend::ActionResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const override;

private:
  sona::strhdl_t m_ConstructorName;
  QualType m_Type;
};

class ADTDecl final : public TypeDecl, public DeclContext {
public:
  ADTDecl(sona::ref_ptr<DeclContext> context,
                sona::strhdl_t const& adtName)
    : TypeDecl(context, DeclKind::DK_ADT, adtName),
      DeclContext(DeclKind::DK_ADT) {}
      
  sona::owner<Backend::ActionResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const override;
};

class UsingDecl final : public TypeDecl {
public:
  UsingDecl(sona::ref_ptr<DeclContext> context,
            sona::strhdl_t const& aliasName,
            QualType aliasee)
    : TypeDecl(context, DeclKind::DK_Using, aliasName), m_Aliasee(aliasee) {}

  /// @note only use this for refilling after dependency resolution
  void FillAliasee(QualType aliasee) noexcept {
    sona_assert(m_Aliasee.GetUnqualTy() == nullptr);
    m_Aliasee = aliasee;
  }

  QualType GetAliasee() const noexcept {
    return m_Aliasee;
  }

  sona::owner<Backend::ActionResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const override;

private:
  QualType m_Aliasee;
};

class EnumeratorDecl final : public Decl {
public:
  EnumeratorDecl(sona::ref_ptr<DeclContext> context,
                 sona::strhdl_t const& enumeratorName,
                 int64_t init)
      : Decl(DeclKind::DK_Enumerator, context),
        m_EnumeratorName(enumeratorName), m_Init(init) {}

  sona::strhdl_t const &GetEnumeratorName() const noexcept {
    return m_EnumeratorName;
  }

  int64_t GetInit() const noexcept {
    return m_Init;
  }
  
  sona::owner<Backend::ActionResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const override;

private:
  sona::strhdl_t m_EnumeratorName;
  int64_t m_Init;
};

class FuncDecl final : public Decl, public DeclContext {
public:
  FuncDecl(sona::ref_ptr<DeclContext> context,
           sona::strhdl_t const& functionName,
           std::vector<sona::ref_ptr<Type const>> &&paramTypes,
           std::vector<sona::strhdl_t> &&paramNames,
           sona::ref_ptr<Type const> retType)
    : Decl(DeclKind::DK_Func, context), DeclContext(DeclKind::DK_Func),
      m_FunctionName(functionName),
      m_ParamTypes(std::move(paramTypes)),
      m_ParamNames(std::move(paramNames)),
      m_RetType(retType) {}

  sona::strhdl_t const& GetName() const noexcept {
    return m_FunctionName;
  }

  std::vector<sona::ref_ptr<Type const>> const&
  GetParamTypes() const noexcept {
    return m_ParamTypes;
  }

  std::vector<sona::strhdl_t> const& GetParamNames() const noexcept {
    return m_ParamNames;
  }

  sona::ref_ptr<Type const> GetRetType() const noexcept {
    return m_RetType;
  }

  bool IsDefinition() const noexcept {
    /// @todo provide a way to link declarations and definitions together.
    return false;
  }
  
  sona::owner<Backend::ActionResult> 
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const override;

private:
  sona::strhdl_t m_FunctionName;
  std::vector<sona::ref_ptr<Type const>> m_ParamTypes;
  std::vector<sona::strhdl_t> m_ParamNames;
  sona::ref_ptr<Type const> m_RetType;
};

class VarDecl final : public Decl {
public:
  VarDecl(sona::ref_ptr<DeclContext> context,
          QualType type, DeclSpec spec, sona::strhdl_t const& varName)
      : Decl(DeclKind::DK_Var, context), m_Type(type), m_DeclSpec(spec),
        m_VarName(varName) {}

  DeclSpec GetDeclSpec() const noexcept { return m_DeclSpec; }
  sona::strhdl_t const &GetVarName() const noexcept { return m_VarName; }
  void SetType(QualType type) noexcept { m_Type = type; }
  QualType GetType() const noexcept { return m_Type; }

  sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::DeclVisitor> visitor) const override;

private:
  QualType m_Type;
  DeclSpec m_DeclSpec;
  sona::strhdl_t m_VarName;
};

// class ParamDecl : public VarDecl {};

// class FieldDecl : public VarDecl {};

} // namespace AST
} // namespace ckx

#endif // AST_DECL_H
