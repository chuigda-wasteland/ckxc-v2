#ifndef CST_CC
#define CST_CC

#include <vector>
#include <string>

#include <Basic/SourceRange.hpp>

#include <Syntax/CSTVisitor.h>

#include <sona/range.hpp>
#include <sona/linq.hpp>
#include <sona/pointer_plus.hpp>
#include <sona/stringref.hpp>

namespace ckx {
namespace Syntax {

class CSTNode {
public:
  enum class CSTNodeKind {
    /// Basic identifier
    CNK_Identifier,

    /// Special structure
    CNK_Import,
    CNK_Export,

    /// Attributes
    CNK_AttributeList,

    /// Forward Declarations
    CNK_ForwardDecl,

    /// Definitions
    CNK_ClassDecl,
    CNK_EnumDecl,
    CNK_ADTDecl,
    CNK_VarDecl,

    /// Function definition is special
    CNK_FuncDecl,

    /** @todo most of these are not completed yet */
    CNK_TypeAliasDecl,
    CNK_LabelDecl,
    CNK_TemplatedDecl,
    CNK_ConceptDecl,

    /// Statements
    CNK_EmptyStmt,
    CNK_ExprStmt,
    CNK_IfStmt,
    CNK_MatchStmt,
    CNK_ForStmt,
    CNK_ForEachStmt,
    CNK_WhileStmt,
    CNK_CompoundStmt,
    CNK_ReturnStmt,

    /// Expressions
    CNK_LiteralExpr,
    CNK_StringLiteralExpr,
    CNK_IdRefExpr,
    CNK_FuncCallExpr,
    CNK_UnaryExpr,
    CNK_BinaryExpr,
    CNK_MixFixExpr,

    /// Type structure
    CNK_BasicType,
    CNK_UserDefinedType,
    CNK_TemplatedType,
    CNK_ComposedType
  };

  CSTNode(CSTNodeKind nodeKind) : m_NodeKind(nodeKind) {}
  virtual ~CSTNode() = 0;

  CSTNodeKind GetNodeKind() const noexcept {
    return m_NodeKind;
  }

private:
  CSTNodeKind m_NodeKind;
};

class CSTAttributeList : public CSTNode {
public:
  class CSTAttribute {
  public:
    CSTAttribute(sona::string_ref const& attributeName,
                 sona::string_ref const& attributeValue)
      : m_AttributeName(attributeName), m_AttributeValue(attributeValue) {}

    CSTAttribute(sona::string_ref const& attributeName)
      : m_AttributeName(attributeName),
        m_AttributeValue(sona::empty_optional()) {}

  private:
    sona::string_ref m_AttributeName;
    sona::optional<sona::string_ref> m_AttributeValue;
  };

  CSTAttributeList(std::vector<CSTAttributeList> &attributes)
    : CSTNode(CSTNode::CSTNodeKind::CNK_AttributeList),
      m_Attributes(std::move(attributes)) {}

  std::vector<CSTAttributeList> const& GetAttributes() const noexcept {
    return m_Attributes;
  }

private:
  std::vector<CSTAttributeList> m_Attributes;
};

class CSTType : public CSTNode {
public:
  CSTType(CSTNodeKind nodeKind) : CSTNode(nodeKind) {}
  // virtual TypeResult accept(CSTTypeVisitor &visitor) = 0;
};

class CSTDecl : public CSTNode {
public:
  CSTDecl(CSTNodeKind nodeKind) : CSTNode(nodeKind) {}
  virtual DeclResult accept(CSTDeclVisitor &visitor) = 0;
};

class CSTStmt : public CSTNode {
public: CSTStmt(CSTNodeKind nodeKind) : CSTNode(nodeKind) {}
};

class CSTExpr : public CSTNode {
public: CSTExpr(CSTNodeKind nodeKind) : CSTNode(nodeKind) {}
};

class CSTBasicType : public CSTType {
public:
  enum class TypeKind {
    TK_Int8, TK_Int16, TK_Int32, TK_Int64,
    TK_UInt8, TK_UInt16, TK_UInt32, TK_UInt64,
    TK_Float, TK_Double, TK_Quad, TK_Bool
  };

  CSTBasicType(TypeKind typeKind)
    : CSTType(CSTNodeKind::CNK_BasicType), m_TypeKind(typeKind) {}

  TypeKind GetTypeKind() const noexcept { return m_TypeKind; }

private:
  TypeKind m_TypeKind;
};

class CSTUserDefinedType : public CSTType {
public:
  CSTUserDefinedType(sona::string_ref name)
    : CSTType(CSTNodeKind::CNK_UserDefinedType), m_Name(name) {}

  sona::string_ref GetName() const noexcept { return m_Name; }

private:
  sona::string_ref m_Name;
};

class CSTTemplatedType : public CSTType {
public:
  using CSTTemplateArg = sona::either<sona::owner<CSTType>,
                                      sona::owner<CSTExpr>>;

  CSTTemplatedType(sona::owner<CSTUserDefinedType> &&rootType,
                   std::vector<CSTTemplateArg> &&templateArgs)
    : CSTType(CSTNodeKind::CNK_TemplatedType),
      m_RootType(std::move(rootType)),
      m_TemplateArgs(std::move(templateArgs)) {}

  sona::ref_ptr<CSTUserDefinedType const> GetRootType() const noexcept {
    return m_RootType.borrow();
  }

  std::vector<CSTTemplateArg> const& GetTemplateArgs() const noexcept {
    return m_TemplateArgs;
  }

private:
  sona::owner<CSTUserDefinedType> m_RootType;
  std::vector<CSTTemplateArg> m_TemplateArgs;
};

class CSTComposedType : public CSTType {
public:
  enum class CSTTypeSpecifier { CTS_Const, CST_Volatile, CST_Pointer, CST_Ref };

  CSTComposedType(sona::owner<CSTType> rootType,
                  std::vector<CSTTypeSpecifier> &&typeSpecifiers)
    : CSTType(CSTNodeKind::CNK_ComposedType),
      m_RootType(std::move(rootType)),
      m_TypeSpecifiers(typeSpecifiers) {}

  sona::ref_ptr<CSTType const> GetRootType() const noexcept {
    return m_RootType.borrow();
  }

  std::vector<CSTTypeSpecifier> const& GetTypeSpecifiers() const noexcept {
    return m_TypeSpecifiers;
  }

private:
  sona::owner<CSTType> m_RootType;
  std::vector<CSTTypeSpecifier> m_TypeSpecifiers;
};

class CSTIdentifier {
public:
  CSTIdentifier(sona::string_ref identifier) : m_Identifier(identifier) {}

  CSTIdentifier(std::vector<sona::string_ref> &&nestedNameSpecifiers,
                sona::string_ref identifier)
    : m_NestedNameSpecifiers(std::move(nestedNameSpecifiers)),
      m_Identifier(identifier) {}

  CSTIdentifier(CSTIdentifier &&that)
    : m_NestedNameSpecifiers(std::move(that.m_NestedNameSpecifiers)),
      m_Identifier(that.m_Identifier) {}

  CSTIdentifier(CSTIdentifier const&) = delete;
  CSTIdentifier& operator=(CSTIdentifier const&) = delete;

  sona::string_ref const& GetIdentifier() const noexcept {
    return m_Identifier;
  }

  std::vector<sona::string_ref> const& GetNestedNameSpecifiers() const noexcept {
    return m_NestedNameSpecifiers;
  }

private:
  std::vector<sona::string_ref> m_NestedNameSpecifiers;
  sona::string_ref m_Identifier;
};

class CSTImport : public CSTNode {
public:
  CSTImport(CSTIdentifier &&importedIdentifier, bool isWeak = false)
    : CSTNode(CSTNodeKind::CNK_Import),
      m_ImportedIdentifier(std::move(importedIdentifier)),
      m_IsWeak(isWeak) {}

  CSTIdentifier const& GetImportedIdentifier() const noexcept {
    return m_ImportedIdentifier;
  }

  bool IsWeak() const noexcept {
    return m_IsWeak;
  }

private:
  CSTIdentifier m_ImportedIdentifier;
  bool m_IsWeak;
};

class CSTExport : public CSTNode {
public:
  CSTExport(sona::owner<CSTDecl> &&node)
    : CSTNode(CSTNodeKind::CNK_Export), m_Node(std::move(node)) {}

  sona::ref_ptr<CSTDecl const> GetExportedDecl() const noexcept {
    return m_Node.borrow();
  }

private:
  sona::owner<CSTDecl> m_Node;
};

class CSTForwardDecl : public CSTDecl {
public:
  enum class ForwardDeclKind { FDK_Class, FDK_Enum, FDK_ADT };
  CSTForwardDecl(ForwardDeclKind fdk, sona::string_ref const& name)
    : CSTDecl(CSTNodeKind::CNK_ForwardDecl),
      m_ForwardDeclKind(fdk), m_Name(name) {}

  ForwardDeclKind GetFDK() const noexcept {
    return m_ForwardDeclKind;
  }

  sona::string_ref const& GetName() const noexcept {
    return m_Name;
  }

private:
  ForwardDeclKind m_ForwardDeclKind;
  sona::string_ref m_Name;
};

class CSTTemplatedDecl : public CSTDecl {
public:
  CSTTemplatedDecl(
      std::vector<sona::either<sona::string_ref, sona::owner<CSTExpr>>> tparams,
      sona::owner<CSTDecl> underlyingDecl)
    : CSTDecl(CSTNodeKind::CNK_TemplatedDecl),
      m_TParams(std::move(tparams)),
      m_UnderlyingDecl(std::move(underlyingDecl)){}

  std::vector<sona::either<sona::string_ref, sona::owner<CSTExpr>>> const&
  GetTemplateParams() const noexcept {
    return m_TParams;
  }

  sona::ref_ptr<CSTDecl const> GetUnderlyingDecl() const noexcept {
    return m_UnderlyingDecl.borrow();
  }

private:
  std::vector<sona::either<sona::string_ref, sona::owner<CSTExpr>>> m_TParams;
  sona::owner<CSTDecl> m_UnderlyingDecl;
};

class CSTClassDecl : public CSTDecl {
public:
  CSTClassDecl(sona::string_ref const& className,
            std::vector<sona::owner<CSTDecl>> &&subDecls)
    : CSTDecl(CSTNodeKind::CNK_ClassDecl),
      m_ClassName(className),
      m_SubDecls(std::move(subDecls)) {}

  sona::string_ref const& GetClassName() const noexcept {
    return m_ClassName;
  }

  auto GetSubDecls() const noexcept {
    return sona::linq::from_container(m_SubDecls).
             transform([](sona::owner<CSTDecl> const& it)
                       { return it.borrow(); });
  }

  DeclResult accept(CSTDeclVisitor &visitor) override;

private:
  sona::string_ref m_ClassName;
  std::vector<sona::owner<CSTDecl>> m_SubDecls;
};

class CSTEnumDecl : public CSTDecl {
public:
  class Enumerator {
  public:
    Enumerator(sona::string_ref const& name,
               sona::optional<int64_t> const& value)
      : m_Name(name), m_Value(value) {}

    sona::string_ref const& GetName() const noexcept {
      return m_Name;
    }

    bool HasValue() const noexcept {
      return m_Value.has_value();
    }

    int GetValue() const noexcept {
      sona_assert(HasValue());
      return m_Value.value();
    }

  private:
    sona::string_ref m_Name;
    sona::optional<int64_t> m_Value;
  };

  DeclResult accept(CSTDeclVisitor &visitor) override;

private:
  std::vector<sona::string_ref> m_Enumerators;
};

class CSTADTDecl : public CSTDecl {
public:
  class DataConstructor {
  public:
    DataConstructor(sona::string_ref const& name,
                    sona::owner<CSTType> &&underlyingType)
      : m_Name(name), m_UnderlyingType(std::move(underlyingType)) {}

    sona::string_ref const& GetName() const noexcept {
      return m_Name;
    }

    sona::ref_ptr<CSTType const> GetUnderlyingType() const noexcept {
      return m_UnderlyingType.borrow();
    }

  private:
    sona::string_ref m_Name;
    sona::owner<CSTType> m_UnderlyingType;
  };

  CSTADTDecl(sona::string_ref const& name,
             std::vector<DataConstructor> &&constructors)
    : CSTDecl(CSTNodeKind::CNK_ADTDecl), m_Name(name),
      m_Constructors(std::move(constructors)) {}

  sona::string_ref const& GetName() const noexcept {
     return m_Name;
  }

  std::vector<DataConstructor> const& GetConstructors() const noexcept {
    return m_Constructors;
  }

  DeclResult accept(CSTDeclVisitor &visitor) override;

private:
  sona::string_ref m_Name;
  std::vector<DataConstructor> m_Constructors;
};

class CSTFuncDecl : public CSTDecl {
public:
  CSTFuncDecl(CSTIdentifier &&name,
              std::vector<sona::owner<CSTType>> &&paramTypes,
              std::vector<sona::string_ref> &&paramNames,
              sona::optional<sona::owner<CSTStmt>> &&funcBody) :
    CSTDecl(CSTNodeKind::CNK_FuncDecl),
    m_Name(std::move(name)),
    m_ParamTypes(std::move(paramTypes)),
    m_ParamNames(std::move(paramNames)),
    m_FuncBody(std::move(funcBody)) {}

  CSTIdentifier const& GetName() const noexcept { return m_Name; }

  auto GetParamTypes() const noexcept {
    return sona::linq::from_container(m_ParamTypes).transform(
          [](sona::owner<CSTType> const& it) { return it.borrow(); });
  }

  bool IsDefinition() const noexcept {
    return m_FuncBody.has_value();
  }

  sona::ref_ptr<CSTStmt const> GetFuncBodyUnsafe() const noexcept {
    return m_FuncBody.value().borrow();
  }

  DeclResult accept(CSTDeclVisitor &visitor) override;

private:
  CSTIdentifier m_Name;
  std::vector<sona::owner<CSTType>> m_ParamTypes;
  std::vector<sona::string_ref> m_ParamNames;
  sona::optional<sona::owner<CSTStmt>> m_FuncBody;
};

class CSTVarDecl : public CSTDecl {
public:
  CSTVarDecl(sona::string_ref const& name, sona::owner<CSTType> type)
    : CSTDecl(CSTNodeKind::CNK_ADTDecl),
      m_Name(name), m_Type(std::move(type)) {}

  sona::string_ref const& GetName() const noexcept {
    return m_Name;
  }

  sona::ref_ptr<CSTType const> GetType() const noexcept {
    return m_Type.borrow();
  }

private:
  sona::string_ref m_Name;
  sona::owner<CSTType> m_Type;
};

} // namespace Syntax;
} // namespace ckx

#endif // CST_CC
