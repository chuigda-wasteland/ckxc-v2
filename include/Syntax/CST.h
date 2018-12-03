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
                 sona::string_ref const& attributeValue,
                 SingleSourceRange nameRange,
                 SingleSourceRange valueRange)
      : m_AttributeName(attributeName),
        m_AttributeValue(attributeValue),
        m_NameRange(nameRange),
        m_ValueRange(valueRange) {}

    CSTAttribute(sona::string_ref const& attributeName,
                 SingleSourceRange nameRange,
                 SingleSourceRange valueRange)
      : m_AttributeName(attributeName),
        m_AttributeValue(sona::empty_optional()),
        m_NameRange(nameRange),
        m_ValueRange(valueRange) {}

    sona::string_ref const& GetAttributeName() const noexcept {
      return m_AttributeName;
    }

    bool HasAttributeValue() const noexcept {
      return m_AttributeValue.has_value();
    }

    sona::string_ref const& GetAttributeValueUnsafe() const noexcept {
      return m_AttributeValue.value();
    }

    SingleSourceRange const& GetNameRange() const noexcept {
      return m_NameRange;
    }

    SingleSourceRange const& GetValueRange() const noexcept {
      return m_ValueRange;
    }

  private:
    sona::string_ref m_AttributeName;
    sona::optional<sona::string_ref> m_AttributeValue;
    SingleSourceRange m_NameRange;
    SingleSourceRange m_ValueRange;
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

  CSTBasicType(TypeKind typeKind, SingleSourceRange const& range)
    : CSTType(CSTNodeKind::CNK_BasicType),
      m_TypeKind(typeKind), m_Range(range) {}

  TypeKind GetTypeKind() const noexcept { return m_TypeKind; }

  SingleSourceRange const& GetSourceRange() const noexcept { return m_Range; }

private:
  TypeKind m_TypeKind;
  SingleSourceRange m_Range;
};

class CSTUserDefinedType : public CSTType {
public:
  CSTUserDefinedType(sona::string_ref name, SingleSourceRange const& range)
    : CSTType(CSTNodeKind::CNK_UserDefinedType),
      m_Name(name), m_Range(range) {}

  sona::string_ref GetName() const noexcept { return m_Name; }

  SingleSourceRange const& GetSourceRange() const noexcept { return m_Range; }

private:
  sona::string_ref m_Name;
  SingleSourceRange m_Range;
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
                  std::vector<CSTTypeSpecifier> &&typeSpecifiers,
                  std::vector<SingleSourceRange> &&typeSpecRanges)
    : CSTType(CSTNodeKind::CNK_ComposedType),
      m_RootType(std::move(rootType)),
      m_TypeSpecifiers(typeSpecifiers),
      m_TypeSpecifierRanges(std::move(typeSpecRanges)) {}

  sona::ref_ptr<CSTType const> GetRootType() const noexcept {
    return m_RootType.borrow();
  }

  std::vector<CSTTypeSpecifier> const& GetTypeSpecifiers() const noexcept {
    return m_TypeSpecifiers;
  }

  std::vector<SingleSourceRange> const&
  GetTypeSpecSourceRanges() const noexcept {
    return m_TypeSpecifierRanges;
  }

private:
  sona::owner<CSTType> m_RootType;
  std::vector<CSTTypeSpecifier> m_TypeSpecifiers;
  std::vector<SingleSourceRange> m_TypeSpecifierRanges;
};

class CSTIdentifier {
public:
  CSTIdentifier(sona::string_ref const& identifier,
                SingleSourceRange const& idRange)
    : m_Identifier(identifier), m_IdRange(idRange) {}

  CSTIdentifier(std::vector<sona::string_ref> &&nestedNameSpecifiers,
                sona::string_ref identifier,
                std::vector<SingleSourceRange> &&nnsRanges,
                SingleSourceRange idRange)
    : m_NestedNameSpecifiers(std::move(nestedNameSpecifiers)),
      m_Identifier(identifier),
      m_NNSRanges(std::move(nnsRanges)),
      m_IdRange(idRange) {}

  CSTIdentifier(CSTIdentifier &&that)
    : m_NestedNameSpecifiers(std::move(that.m_NestedNameSpecifiers)),
      m_Identifier(that.m_Identifier),
      m_NNSRanges(std::move(that.m_NNSRanges)),
      m_IdRange(that.m_IdRange) {}

  CSTIdentifier(CSTIdentifier const&) = delete;
  CSTIdentifier& operator=(CSTIdentifier const&) = delete;

  sona::string_ref const& GetIdentifier() const noexcept {
    return m_Identifier;
  }

  SingleSourceRange const& GetIdSourceRange() const noexcept {
    return m_IdRange;
  }

  std::vector<sona::string_ref> const&
  GetNestedNameSpecifiers() const noexcept {
    return m_NestedNameSpecifiers;
  }

  std::vector<SingleSourceRange> const& GetNNSSourceRanges() const noexcept {
    return m_NNSRanges;
  }

private:
  std::vector<sona::string_ref> m_NestedNameSpecifiers;
  sona::string_ref m_Identifier;
  std::vector<SingleSourceRange> m_NNSRanges;
  SingleSourceRange m_IdRange;
};

class CSTImport : public CSTNode {
public:
  CSTImport(CSTIdentifier &&importedIdentifier,
            SingleSourceRange const& importRange)
    : CSTNode(CSTNodeKind::CNK_Import),
      m_ImportedIdentifier(std::move(importedIdentifier)),
      m_ImportRange(importRange),
      m_IsWeak(false),
      m_WeakRange(0, 0, 0) {}

  CSTImport(CSTIdentifier &&importedIdentifier,
            SingleSourceRange const& importRange,
            std::true_type /* isWeakImport */,
            SingleSourceRange const& weakRange)
    : CSTNode(CSTNodeKind::CNK_Import),
      m_ImportedIdentifier(std::move(importedIdentifier)),
      m_ImportRange(importRange),
      m_IsWeak(true),
      m_WeakRange(weakRange) {}

  CSTIdentifier const& GetImportedIdentifier() const noexcept {
    return m_ImportedIdentifier;
  }

  SingleSourceRange const& GetImportSourceRange() const noexcept {
    return m_ImportRange;
  }

  bool IsWeak() const noexcept {
    return m_IsWeak;
  }

  SingleSourceRange const& GetWeakSourceRangeUnsafe() const noexcept {
    sona_assert(IsWeak());
    return m_WeakRange;
  }

private:
  CSTIdentifier m_ImportedIdentifier;
  SingleSourceRange m_ImportRange;
  bool m_IsWeak;
  SingleSourceRange m_WeakRange;
};

class CSTExport : public CSTNode {
public:
  CSTExport(sona::owner<CSTDecl> &&node, SingleSourceRange exportRange)
    : CSTNode(CSTNodeKind::CNK_Export), m_Node(std::move(node)),
      m_ExportRange(exportRange) {}

  sona::ref_ptr<CSTDecl const> GetExportedDecl() const noexcept {
    return m_Node.borrow();
  }

  SingleSourceRange const& GetExportSourceRange() const noexcept {
    return m_ExportRange;
  }

private:
  sona::owner<CSTDecl> m_Node;
  SingleSourceRange m_ExportRange;
};

class CSTForwardDecl : public CSTDecl {
public:
  enum class ForwardDeclKind { FDK_Class, FDK_Enum, FDK_ADT };
  CSTForwardDecl(ForwardDeclKind fdk, sona::string_ref const& name,
                 SingleSourceRange const& keywordRange,
                 SingleSourceRange const& nameRange)
    : CSTDecl(CSTNodeKind::CNK_ForwardDecl),
      m_ForwardDeclKind(fdk), m_Name(name),
      m_KeywordRange(keywordRange),
      m_NameRange(nameRange) {}

  ForwardDeclKind GetFDK() const noexcept {
    return m_ForwardDeclKind;
  }

  sona::string_ref const& GetName() const noexcept {
    return m_Name;
  }

  SingleSourceRange const& GetKeywordSourceRange() const noexcept {
    return m_KeywordRange;
  }

  SingleSourceRange const& GetNameSourceRange() const noexcept {
    return m_NameRange;
  }

private:
  ForwardDeclKind m_ForwardDeclKind;
  sona::string_ref m_Name;
  SingleSourceRange m_KeywordRange;
  SingleSourceRange m_NameRange;
};

class CSTTemplatedDecl : public CSTDecl {
public:
  CSTTemplatedDecl(
      std::vector<sona::either<sona::string_ref, sona::owner<CSTExpr>>> tparams,
      sona::owner<CSTDecl> underlyingDecl,
      SingleSourceRange templateRange)
    : CSTDecl(CSTNodeKind::CNK_TemplatedDecl),
      m_TParams(std::move(tparams)),
      m_UnderlyingDecl(std::move(underlyingDecl)),
      m_TemplateRange(templateRange) {}

  std::vector<sona::either<sona::string_ref, sona::owner<CSTExpr>>> const&
  GetTemplateParams() const noexcept {
    return m_TParams;
  }

  sona::ref_ptr<CSTDecl const> GetUnderlyingDecl() const noexcept {
    return m_UnderlyingDecl.borrow();
  }

  SingleSourceRange const& GetTemplateSourceRange() const noexcept {
    return m_TemplateRange;
  }

private:
  std::vector<sona::either<sona::string_ref, sona::owner<CSTExpr>>> m_TParams;
  sona::owner<CSTDecl> m_UnderlyingDecl;
  SingleSourceRange m_TemplateRange;
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
               int64_t value,
               SingleSourceRange nameRange,
               SourceLocation eqLoc,
               SingleSourceRange valueRange)
      : m_Name(name), m_Value(value),
        m_NameRange(nameRange), m_EqLoc(eqLoc), m_ValueRange(valueRange) {}

    Enumerator(const sona::string_ref &name, SingleSourceRange nameRange)
      : m_Name(name), m_Value(sona::empty_optional()),
        m_NameRange(nameRange), m_EqLoc(0, 0), m_ValueRange(0, 0, 0) {}

    sona::string_ref const& GetName() const noexcept {
      return m_Name;
    }

    SingleSourceRange const& GetNameSourceRange() const noexcept {
      return m_NameRange;
    }

    bool HasValue() const noexcept {
      return m_Value.has_value();
    }

    int GetValueUnsafe() const noexcept {
      sona_assert(HasValue());
      return m_Value.value();
    }

    SourceLocation const& GetEqSourceLocUnsafe() const noexcept {
      sona_assert(HasValue());
      return m_EqLoc;
    }

    SingleSourceRange const& GetValueSourceRangeUnsafe() const noexcept {
      sona_assert(HasValue());
      return m_ValueRange;
    }

  private:
    sona::string_ref m_Name;
    sona::optional<int64_t> m_Value;
    SingleSourceRange m_NameRange;
    SourceLocation m_EqLoc;
    SingleSourceRange m_ValueRange;
  };

  CSTEnumDecl(sona::string_ref const& name,
              std::vector<Enumerator> &&enumerators,
              SingleSourceRange const& enumRange,
              SingleSourceRange const& nameRange)
    : CSTDecl(CSTNodeKind::CNK_EnumDecl),
      m_Name(name), m_Enumerators(std::move(enumerators)),
      m_EnumRange(enumRange), m_NameRange(nameRange) {}

  sona::string_ref const& GetName() const noexcept {
    return m_Name;
  }

  std::vector<Enumerator> const& GetEnumerators() const noexcept {
    return m_Enumerators;
  }

  SingleSourceRange const& GetEnumSourceRange() const noexcept {
    return m_EnumRange;
  }

  SingleSourceRange const& GetNameSourceRange() const noexcept {
    return m_NameRange;
  }

  DeclResult accept(CSTDeclVisitor &visitor) override;

private:
  sona::string_ref m_Name;
  std::vector<Enumerator> m_Enumerators;
  SingleSourceRange m_EnumRange, m_NameRange;
};

class CSTADTDecl : public CSTDecl {
public:
  class DataConstructor {
  public:
    DataConstructor(sona::string_ref const& name,
                    sona::owner<CSTType> &&underlyingType,
                    SingleSourceRange const& nameRange)
      : m_Name(name), m_UnderlyingType(std::move(underlyingType)),
        m_NameRange(nameRange) {}

    DataConstructor(sona::string_ref const& name,
                    SingleSourceRange const& nameRange)
      : m_Name(name), m_UnderlyingType(nullptr), m_NameRange(nameRange) {}

    sona::string_ref const& GetName() const noexcept {
      return m_Name;
    }

    sona::ref_ptr<CSTType const> GetUnderlyingType() const noexcept {
      return m_UnderlyingType.borrow();
    }

    SingleSourceRange const& GetNameSourceRange() const noexcept {
      return m_NameRange;
    }

  private:
    sona::string_ref m_Name;
    sona::owner<CSTType> m_UnderlyingType;
    SingleSourceRange m_NameRange;
  };

  CSTADTDecl(sona::string_ref const& name,
             std::vector<DataConstructor> &&constructors,
             SingleSourceRange const& enumClassRange,
             SingleSourceRange const& nameRange)
    : CSTDecl(CSTNodeKind::CNK_ADTDecl), m_Name(name),
      m_Constructors(std::move(constructors)),
      m_EnumClassRange(enumClassRange),
      m_NameRange(nameRange) {}

  sona::string_ref const& GetName() const noexcept {
     return m_Name;
  }

  std::vector<DataConstructor> const& GetConstructors() const noexcept {
    return m_Constructors;
  }

  SingleSourceRange const& GetKeywordSourceRange() const noexcept {
    return m_EnumClassRange;
  }

  SingleSourceRange const& GetNameSourceRange() const noexcept {
    return m_NameRange;
  }

  DeclResult accept(CSTDeclVisitor &visitor) override;

private:
  sona::string_ref m_Name;
  std::vector<DataConstructor> m_Constructors;
  SingleSourceRange m_EnumClassRange;
  SingleSourceRange m_NameRange;

};

class CSTFuncDecl : public CSTDecl {
public:
  CSTFuncDecl(CSTIdentifier &&name,
              std::vector<sona::owner<CSTType>> &&paramTypes,
              std::vector<sona::string_ref> &&paramNames,
              sona::optional<sona::owner<CSTStmt>> &&funcBody,
              SingleSourceRange funcRange,
              SingleSourceRange nameRange) :
    CSTDecl(CSTNodeKind::CNK_FuncDecl),
    m_Name(std::move(name)),
    m_ParamTypes(std::move(paramTypes)),
    m_ParamNames(std::move(paramNames)),
    m_FuncBody(std::move(funcBody)),
    m_FuncRange(funcRange), m_NameRange(nameRange) {}

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

  SingleSourceRange const& GetKeywordSourceRange() const noexcept {
    return m_FuncRange;
  }

  SingleSourceRange const& GetNameSourceRange() const noexcept {
    return m_NameRange;
  }

  DeclResult accept(CSTDeclVisitor &visitor) override;

private:
  CSTIdentifier m_Name;
  std::vector<sona::owner<CSTType>> m_ParamTypes;
  std::vector<sona::string_ref> m_ParamNames;
  sona::optional<sona::owner<CSTStmt>> m_FuncBody;
  SingleSourceRange m_FuncRange, m_NameRange;
};

class CSTVarDecl : public CSTDecl {
public:
  CSTVarDecl(sona::string_ref const& name, sona::owner<CSTType> type,
             SingleSourceRange const& defRange,
             SingleSourceRange const& nameRange)
    : CSTDecl(CSTNodeKind::CNK_VarDecl),
      m_Name(name), m_Type(std::move(type)),
      m_DefRange(defRange), m_NameRange(nameRange) {}

  sona::string_ref const& GetName() const noexcept {
    return m_Name;
  }

  sona::ref_ptr<CSTType const> GetType() const noexcept {
    return m_Type.borrow();
  }

  SingleSourceRange const& GetKeywordSourceRange() const noexcept {
    return m_DefRange;
  }

  SingleSourceRange const& GetNameSourceRange() const noexcept {
    return m_NameRange;
  }

private:
  sona::string_ref m_Name;
  sona::owner<CSTType> m_Type;
  SingleSourceRange m_DefRange, m_NameRange;
};

} // namespace Syntax;
} // namespace ckx

#endif // CST_CC
