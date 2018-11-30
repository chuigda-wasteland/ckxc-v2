#ifndef CST_CC
#define CST_CC

#include <vector>
#include <string>

#include <Basic/SourceRange.hpp>

#include <sona/range.hpp>
#include <sona/linq.hpp>
#include <sona/pointer_plus.hpp>

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

    /// Declarations(Definitions)
    CNK_ClassDecl,
    CNK_EnumDecl,
    CNK_ADTDecl,
    CNK_FuncDecl,
    CNK_VarDecl,

    /** @todo most of these are not completed yet */
    CNK_TypeAliasDecl,
    CNK_LabelDecl,
    CNK_TemplateDecl,
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

    /// Expressions
    CNK_LiteralExpr,
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

class CSTType : public CSTNode {
public: CSTType(CSTNodeKind nodeKind) : CSTNode(nodeKind) {}
};

class CSTDecl : public CSTNode {
public: CSTDecl(CSTNodeKind nodeKind) : CSTNode(nodeKind) {}
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
  CSTUserDefinedType(std::string &&name)
    : CSTType(CSTNodeKind::CNK_UserDefinedType), m_Name(std::move(name)) {}

  std::string const& GetName() const noexcept { return m_Name; }

private:
  std::string m_Name;
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
  CSTIdentifier(std::string &&identifier)
    : m_Identifier(std::move(identifier)) {}

  CSTIdentifier(std::vector<std::string> &&nestedNameSpecifiers,
                std::string &&identifier)
    : m_NestedNameSpecifiers(std::move(nestedNameSpecifiers)),
      m_Identifier(std::move(identifier)) {}

  CSTIdentifier(CSTIdentifier &&that)
    : m_NestedNameSpecifiers(std::move(that.m_NestedNameSpecifiers)),
      m_Identifier(std::move(that.m_Identifier)) {}

  CSTIdentifier(CSTIdentifier const&) = delete;
  CSTIdentifier& operator=(CSTIdentifier const&) = delete;

  std::string const& GetIdentifier() const noexcept {
    return m_Identifier;
  }

  std::vector<std::string> const& GetNestedNameSpecifiers() const noexcept {
    return m_NestedNameSpecifiers;
  }

private:
  std::vector<std::string> m_NestedNameSpecifiers;
  std::string m_Identifier;
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

class CSTClassDecl : public CSTDecl {
public:
  CSTClassDecl(CSTIdentifier &&className,
            std::vector<sona::owner<CSTDecl>> &&subDecls)
    : CSTDecl(CSTNodeKind::CNK_ClassDecl),
      m_ClassName(std::move(className)),
      m_SubDecls(std::move(subDecls)) {}

  CSTIdentifier const& GetClassName() const noexcept {
    return m_ClassName;
  }

  auto GetSubDecls() const noexcept {
    return sona::linq::from_container(m_SubDecls).
             transform([](sona::owner<CSTDecl> const& it)
                       { return it.borrow(); });
  }

private:
  CSTIdentifier m_ClassName;
  std::vector<sona::owner<CSTDecl>> m_SubDecls;
};

class CSTEnumDecl : public CSTDecl {
public:
  class Enumerator {
  public:
    Enumerator(std::string &&name, sona::optional<int64_t> const& value)
      : m_Name(name), m_Value(value) {}

    std::string const& GetName() const noexcept {
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
    std::string m_Name;
    sona::optional<int64_t> m_Value;
  };

private:
  std::vector<std::string> m_Enumerators;
};

class CSTADTDecl : public CSTDecl {
public:
  class DataConstructor {
  public:
    DataConstructor(std::string &&name, sona::owner<CSTType> &&underlyingType)
      : m_Name(name), m_UnderlyingType(std::move(underlyingType)) {}

    std::string const& GetName() const noexcept {
      return m_Name;
    }

    sona::ref_ptr<CSTType const> GetUnderlyingType() const noexcept {
      return m_UnderlyingType.borrow();
    }

  private:
    std::string m_Name;
    sona::owner<CSTType> m_UnderlyingType;
  };

  CSTADTDecl(std::string &&name, std::vector<DataConstructor> &&constructors)
    : CSTDecl(CSTNodeKind::CNK_ADTDecl), m_Name(std::move(name)),
      m_Constructors(std::move(constructors)) {}

  std::vector<DataConstructor> const& GetConstructors() const noexcept {
    return m_Constructors;
  }

private:
  std::string m_Name;
  std::vector<DataConstructor> m_Constructors;
};

class CSTFuncDecl : public CSTDecl {
public:
  CSTFuncDecl(std::string &&name,
              std::vector<sona::owner<CSTType>> &&paramTypes,
              std::vector<std::string> &&paramNames) :
    CSTDecl(CSTNodeKind::CNK_FuncDecl),
    m_Name(std::move(name)),
    m_ParamTypes(std::move(paramTypes)),
    m_ParamNames(std::move(paramNames)) {}

  std::string const& GetName() const noexcept { return m_Name; }

  auto GetParamTypes() const noexcept {
    return sona::linq::from_container(m_ParamTypes).transform(
          [](sona::owner<CSTType> const& it) { return it.borrow(); });
  }

private:
  std::string m_Name;
  std::vector<sona::owner<CSTType>> m_ParamTypes;
  std::vector<std::string> m_ParamNames;
};

} // namespace Syntax;
} // namespace ckx

#endif // CST_CC
