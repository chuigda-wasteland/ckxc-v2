#ifndef CST_CC
#define CST_CC

#include <vector>
#include <string>

#include <Basic/SourceRange.hpp>

#include <sona/range.hpp>
#include <sona/linq.hpp>
#include <sona/pointer_plus.hpp>
#include <sona/stringref.hpp>

namespace ckx {
namespace Syntax {

class Node {
public:
  enum class NodeKind {
    #define CST_TRANSUNIT(name) CNK_##name,
    #define CST_MISC(name) CNK_##name,
    #define CST_TYPE(name) CNK_##name,
    #define CST_DECL(name) CNK_##name,
    #define CST_STMT(name) CNK_##name,
    #define CST_EXPR(name) CNK_##name,

    #include "Syntax/CSTNodeDefs.def"
  };

  Node(NodeKind nodeKind) : m_NodeKind(nodeKind) {}
  virtual ~Node() {}

  NodeKind GetNodeKind() const noexcept {
    return m_NodeKind;
  }

private:
  NodeKind m_NodeKind;
};

class AttributeList : public Node {
public:
  class Attribute {
  public:
    Attribute(sona::string_ref const& attributeName,
              sona::string_ref const& attributeValue,
              SingleSourceRange nameRange,
              SingleSourceRange valueRange)
      : m_AttributeName(attributeName),
        m_AttributeValue(attributeValue),
        m_NameRange(nameRange),
        m_ValueRange(valueRange) {}

    Attribute(sona::string_ref const& attributeName,
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

  AttributeList(std::vector<AttributeList> &attributes)
    : Node(Node::NodeKind::CNK_AttributeList),
      m_Attributes(std::move(attributes)) {}

  std::vector<AttributeList> const& GetAttributes() const noexcept {
    return m_Attributes;
  }

private:
  std::vector<AttributeList> m_Attributes;
};

class Type : public Node {
public:
  Type(NodeKind nodeKind) : Node(nodeKind) {}
};

class Decl : public Node {
public:
  Decl(NodeKind nodeKind) : Node(nodeKind) {}
};

class Stmt : public Node {
public: Stmt(NodeKind nodeKind) : Node(nodeKind) {}
};

class Expr : public Node {
public: Expr(NodeKind nodeKind) : Node(nodeKind) {}
};

class BasicType : public Type {
public:
  enum class TypeKind {
    TK_Int8, TK_Int16, TK_Int32, TK_Int64,
    TK_UInt8, TK_UInt16, TK_UInt32, TK_UInt64,
    TK_Float, TK_Double, TK_Quad, TK_Bool,
    TK_Void
  };

  BasicType(TypeKind typeKind, SingleSourceRange const& range)
    : Type(NodeKind::CNK_BasicType),
      m_TypeKind(typeKind), m_Range(range) {}

  TypeKind GetTypeKind() const noexcept { return m_TypeKind; }

  SingleSourceRange const& GetSourceRange() const noexcept { return m_Range; }

private:
  TypeKind m_TypeKind;
  SingleSourceRange m_Range;
};

class UserDefinedType : public Type {
public:
  UserDefinedType(sona::string_ref name, SingleSourceRange const& range)
    : Type(NodeKind::CNK_UserDefinedType),
      m_Name(name), m_Range(range) {}

  sona::string_ref GetName() const noexcept { return m_Name; }

  SingleSourceRange const& GetSourceRange() const noexcept { return m_Range; }

private:
  sona::string_ref m_Name;
  SingleSourceRange m_Range;
};

class TemplatedType : public Type {
public:
  using TemplateArg = sona::either<sona::owner<Type>,
                                      sona::owner<Expr>>;

  TemplatedType(sona::owner<UserDefinedType> &&rootType,
                   std::vector<TemplateArg> &&templateArgs)
    : Type(NodeKind::CNK_TemplatedType),
      m_RootType(std::move(rootType)),
      m_TemplateArgs(std::move(templateArgs)) {}

  sona::ref_ptr<UserDefinedType const> GetRootType() const noexcept {
    return m_RootType.borrow();
  }

  std::vector<TemplateArg> const& GetTemplateArgs() const noexcept {
    return m_TemplateArgs;
  }

private:
  sona::owner<UserDefinedType> m_RootType;
  std::vector<TemplateArg> m_TemplateArgs;
};

class ComposedType : public Type {
public:
  enum class TypeSpecifier {
    CTS_Const, CTS_Volatile, CTS_Pointer, CTS_Ref, CTS_RvRef
  };

  ComposedType(sona::owner<Type> rootType,
               std::vector<TypeSpecifier> &&typeSpecifiers,
               std::vector<SingleSourceRange> &&typeSpecRanges)
    : Type(NodeKind::CNK_ComposedType),
      m_RootType(std::move(rootType)),
      m_TypeSpecifiers(typeSpecifiers),
      m_TypeSpecifierRanges(std::move(typeSpecRanges)) {}

  sona::ref_ptr<Type const> GetRootType() const noexcept {
    return m_RootType.borrow();
  }

  std::vector<TypeSpecifier> const& GetTypeSpecifiers() const noexcept {
    return m_TypeSpecifiers;
  }

  std::vector<SingleSourceRange> const&
  GetTypeSpecSourceRanges() const noexcept {
    return m_TypeSpecifierRanges;
  }

private:
  sona::owner<Type> m_RootType;
  std::vector<TypeSpecifier> m_TypeSpecifiers;
  std::vector<SingleSourceRange> m_TypeSpecifierRanges;
};

class Identifier {
public:
  Identifier(sona::string_ref const& identifier,
             SingleSourceRange const& idRange)
    : m_Identifier(identifier), m_IdRange(idRange) {}

  Identifier(std::vector<sona::string_ref> &&nestedNameSpecifiers,
             sona::string_ref identifier,
             std::vector<SingleSourceRange> &&nnsRanges,
             SingleSourceRange idRange)
    : m_NestedNameSpecifiers(std::move(nestedNameSpecifiers)),
      m_Identifier(identifier),
      m_NNSRanges(std::move(nnsRanges)),
      m_IdRange(idRange) {}

  Identifier(Identifier &&that)
    : m_NestedNameSpecifiers(std::move(that.m_NestedNameSpecifiers)),
      m_Identifier(that.m_Identifier),
      m_NNSRanges(std::move(that.m_NNSRanges)),
      m_IdRange(that.m_IdRange) {}

  Identifier(Identifier const&) = delete;
  Identifier& operator=(Identifier const&) = delete;

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

class Import : public Node {
public:
  Import(Identifier &&importedIdentifier,
         SingleSourceRange const& importRange)
    : Node(NodeKind::CNK_Import),
      m_ImportedIdentifier(std::move(importedIdentifier)),
      m_ImportRange(importRange),
      m_IsWeak(false),
      m_WeakRange(0, 0, 0) {}

  Import(Identifier &&importedIdentifier,
         SingleSourceRange const& importRange,
         std::true_type /* isWeakImport */,
         SingleSourceRange const& weakRange)
    : Node(NodeKind::CNK_Import),
      m_ImportedIdentifier(std::move(importedIdentifier)),
      m_ImportRange(importRange),
      m_IsWeak(true),
      m_WeakRange(weakRange) {}

  Identifier const& GetImportedIdentifier() const noexcept {
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
  Identifier m_ImportedIdentifier;
  SingleSourceRange m_ImportRange;
  bool m_IsWeak;
  SingleSourceRange m_WeakRange;
};

class Export : public Node {
public:
  Export(sona::owner<Decl> &&node, SingleSourceRange exportRange)
    : Node(NodeKind::CNK_Export), m_Node(std::move(node)),
      m_ExportRange(exportRange) {}

  sona::ref_ptr<Decl const> GetExportedDecl() const noexcept {
    return m_Node.borrow();
  }

  SingleSourceRange const& GetExportSourceRange() const noexcept {
    return m_ExportRange;
  }

private:
  sona::owner<Decl> m_Node;
  SingleSourceRange m_ExportRange;
};

class ForwardDecl : public Decl {
public:
  enum class ForwardDeclKind { FDK_Class, FDK_Enum, FDK_ADT };
  ForwardDecl(ForwardDeclKind fdk, sona::string_ref const& name,
                 SingleSourceRange const& keywordRange,
                 SingleSourceRange const& nameRange)
    : Decl(NodeKind::CNK_ForwardDecl),
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

class TemplatedDecl : public Decl {
public:
  TemplatedDecl(
      std::vector<sona::either<sona::string_ref, sona::owner<Expr>>> tparams,
      sona::owner<Decl> underlyingDecl,
      SingleSourceRange templateRange)
    : Decl(NodeKind::CNK_TemplatedDecl),
      m_TParams(std::move(tparams)),
      m_UnderlyingDecl(std::move(underlyingDecl)),
      m_TemplateRange(templateRange) {}

  std::vector<sona::either<sona::string_ref, sona::owner<Expr>>> const&
  GetTemplateParams() const noexcept {
    return m_TParams;
  }

  sona::ref_ptr<Decl const> GetUnderlyingDecl() const noexcept {
    return m_UnderlyingDecl.borrow();
  }

  SingleSourceRange const& GetTemplateSourceRange() const noexcept {
    return m_TemplateRange;
  }

private:
  std::vector<sona::either<sona::string_ref, sona::owner<Expr>>> m_TParams;
  sona::owner<Decl> m_UnderlyingDecl;
  SingleSourceRange m_TemplateRange;
};

class ClassDecl : public Decl {
public:
  ClassDecl(sona::string_ref const& className,
            std::vector<sona::owner<Decl>> &&subDecls,
            SingleSourceRange const& classKwdRange,
            SingleSourceRange const& classNameRange)
    : Decl(NodeKind::CNK_ClassDecl),
      m_ClassName(className),
      m_SubDecls(std::move(subDecls)),
      m_ClassKwdRange(classKwdRange),
      m_ClassNameRange(classNameRange) {}

  sona::string_ref const& GetClassName() const noexcept {
    return m_ClassName;
  }

  auto GetSubDecls() const noexcept {
    return sona::linq::from_container(m_SubDecls).
             transform([](sona::owner<Decl> const& it)
                       { return it.borrow(); });
  }

  SourceRange const& GetKeywordRange() const noexcept {
    return m_ClassKwdRange;
  }

  SourceRange const& GetNameRange() const noexcept {
    return m_ClassNameRange;
  }

private:
  sona::string_ref m_ClassName;
  std::vector<sona::owner<Decl>> m_SubDecls;
  SourceRange m_ClassKwdRange;
  SourceRange m_ClassNameRange;
};

class EnumDecl : public Decl {
public:
  class Enumerator {
  public:
    Enumerator(sona::string_ref const& name,
               int64_t value,
               SingleSourceRange nameRange,
               SingleSourceRange eqLoc,
               SingleSourceRange valueRange)
      : m_Name(name), m_Value(value),
        m_NameRange(nameRange), m_EqLoc(eqLoc), m_ValueRange(valueRange) {}

    Enumerator(const sona::string_ref &name, SingleSourceRange nameRange)
      : m_Name(name), m_Value(sona::empty_optional()),
        m_NameRange(nameRange), m_EqLoc(0, 0, 0), m_ValueRange(0, 0, 0) {}

    sona::string_ref const& GetName() const noexcept {
      return m_Name;
    }

    SingleSourceRange const& GetNameSourceRange() const noexcept {
      return m_NameRange;
    }

    bool HasValue() const noexcept {
      return m_Value.has_value();
    }

    int64_t GetValueUnsafe() const noexcept {
      sona_assert(HasValue());
      return m_Value.value();
    }

    SingleSourceRange const& GetEqSourceLocUnsafe() const noexcept {
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
    SingleSourceRange m_EqLoc;
    SingleSourceRange m_ValueRange;
  };

  EnumDecl(sona::string_ref const& name,
           std::vector<Enumerator> &&enumerators,
           SingleSourceRange const& enumRange,
           SingleSourceRange const& nameRange)
    : Decl(NodeKind::CNK_EnumDecl),
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

private:
  sona::string_ref m_Name;
  std::vector<Enumerator> m_Enumerators;
  SingleSourceRange m_EnumRange, m_NameRange;
};

class ADTDecl : public Decl {
public:
  class DataConstructor {
  public:
    DataConstructor(sona::string_ref const& name,
                    sona::owner<Type> &&underlyingType,
                    SingleSourceRange const& nameRange)
      : m_Name(name), m_UnderlyingType(std::move(underlyingType)),
        m_NameRange(nameRange) {}

    DataConstructor(sona::string_ref const& name,
                    SingleSourceRange const& nameRange)
      : m_Name(name), m_UnderlyingType(nullptr), m_NameRange(nameRange) {}

    sona::string_ref const& GetName() const noexcept {
      return m_Name;
    }

    sona::ref_ptr<Type const> GetUnderlyingType() const noexcept {
      return m_UnderlyingType.borrow();
    }

    SingleSourceRange const& GetNameSourceRange() const noexcept {
      return m_NameRange;
    }

  private:
    sona::string_ref m_Name;
    sona::owner<Type> m_UnderlyingType;
    SingleSourceRange m_NameRange;
  };

  ADTDecl(sona::string_ref const& name,
          std::vector<DataConstructor> &&constructors,
          SingleSourceRange const& enumRange,
          SingleSourceRange const& classRange,
          SingleSourceRange const& nameRange)
    : Decl(NodeKind::CNK_ADTDecl), m_Name(name),
      m_Constructors(std::move(constructors)),
      m_EnumRange(enumRange),
      m_ClassRange(classRange),
      m_NameRange(nameRange) {}

  sona::string_ref const& GetName() const noexcept {
     return m_Name;
  }

  std::vector<DataConstructor> const& GetConstructors() const noexcept {
    return m_Constructors;
  }

  SingleSourceRange const& GetEnumRange() const noexcept {
    return m_EnumRange;
  }

  SingleSourceRange const& GetClassRange() const noexcept {
    return m_ClassRange;
  }

  SingleSourceRange const& GetNameSourceRange() const noexcept {
    return m_NameRange;
  }

private:
  sona::string_ref m_Name;
  std::vector<DataConstructor> m_Constructors;
  SingleSourceRange m_EnumRange;
  SingleSourceRange m_ClassRange;
  SingleSourceRange m_NameRange;
};

class FuncDecl : public Decl {
public:
  FuncDecl(sona::string_ref const& name,
           std::vector<sona::owner<Type>> &&paramTypes,
           std::vector<sona::string_ref> &&paramNames,
           sona::owner<Type> &&retType,
           sona::optional<sona::owner<Stmt>> &&funcBody,
           SingleSourceRange funcRange,
           SingleSourceRange nameRange) :
    Decl(NodeKind::CNK_FuncDecl),
    m_Name(name),
    m_ParamTypes(std::move(paramTypes)),
    m_ParamNames(std::move(paramNames)),
    m_RetType(std::move(retType)),
    m_FuncBody(std::move(funcBody)),
    m_FuncRange(funcRange), m_NameRange(nameRange) {}

  sona::string_ref const& GetName() const noexcept { return m_Name; }

  auto GetParamTypes() const noexcept {
    return sona::linq::from_container(m_ParamTypes).transform(
          [](sona::owner<Type> const& it) { return it.borrow(); });
  }

  std::vector<sona::string_ref> const& GetParamNames() const noexcept {
    return m_ParamNames;
  }

  sona::ref_ptr<Type const> GetReturnType() const noexcept {
    return m_RetType.borrow();
  }

  bool IsDefinition() const noexcept {
    return m_FuncBody.has_value();
  }

  sona::ref_ptr<Stmt const> GetFuncBodyUnsafe() const noexcept {
    return m_FuncBody.value().borrow();
  }

  SingleSourceRange const& GetKeywordSourceRange() const noexcept {
    return m_FuncRange;
  }

  SingleSourceRange const& GetNameSourceRange() const noexcept {
    return m_NameRange;
  }

private:
  sona::string_ref m_Name;
  std::vector<sona::owner<Type>> m_ParamTypes;
  std::vector<sona::string_ref> m_ParamNames;
  sona::owner<Type> m_RetType;
  sona::optional<sona::owner<Stmt>> m_FuncBody;
  SingleSourceRange m_FuncRange, m_NameRange;
};

class VarDecl : public Decl {
public:
  VarDecl(sona::string_ref const& name, sona::owner<Type> type,
          SingleSourceRange const& defRange,
          SingleSourceRange const& nameRange)
    : Decl(NodeKind::CNK_VarDecl),
      m_Name(name), m_Type(std::move(type)),
      m_DefRange(defRange), m_NameRange(nameRange) {}

  sona::string_ref const& GetName() const noexcept {
    return m_Name;
  }

  sona::ref_ptr<Type const> GetType() const noexcept {
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
  sona::owner<Type> m_Type;
  SingleSourceRange m_DefRange, m_NameRange;
};

class LiteralExpr : public Expr {
public:
  LiteralExpr(std::int64_t intValue, BasicType::TypeKind ofType,
              SourceRange const& range)
    : Expr(NodeKind::CNK_LiteralExpr), m_TypeKind(ofType), m_Range(range) {
    m_Value.IntValue = intValue;
  }

  LiteralExpr(std::uint64_t uIntValue, BasicType::TypeKind ofType,
              SourceRange const& range)
    : Expr(NodeKind::CNK_LiteralExpr), m_TypeKind(ofType), m_Range(range) {
    m_Value.UIntValue = uIntValue;
  }

  LiteralExpr(double floatValue, BasicType::TypeKind ofType,
              SourceRange const& range)
    : Expr(NodeKind::CNK_LiteralExpr), m_TypeKind(ofType), m_Range(range) {
    m_Value.FloatValue = floatValue;
  }

  BasicType::TypeKind GetLiteralTypeKind() const noexcept {
    return m_TypeKind;
  }

  std::int64_t GetAsIntUnsafe() const noexcept {
    sona_assert(m_TypeKind == BasicType::TypeKind::TK_Int8
                || m_TypeKind == BasicType::TypeKind::TK_Int16
                || m_TypeKind == BasicType::TypeKind::TK_Int32
                || m_TypeKind == BasicType::TypeKind::TK_Int64);
    return m_Value.IntValue;
  }

  std::uint64_t GetAsUIntUnsafe() const noexcept {
    sona_assert(m_TypeKind == BasicType::TypeKind::TK_UInt8
                || m_TypeKind == BasicType::TypeKind::TK_UInt16
                || m_TypeKind == BasicType::TypeKind::TK_UInt32
                || m_TypeKind == BasicType::TypeKind::TK_UInt64);
    return m_Value.UIntValue;
  }

  double GetAsFloatUnsafe() const noexcept {
    sona_assert(m_TypeKind == BasicType::TypeKind::TK_Float
                || m_TypeKind == BasicType::TypeKind::TK_Double);
    return m_Value.FloatValue;
  }

  SourceRange const& GetSourceRange() const noexcept {
    return m_Range;
  }

private:
  union {
    std::int64_t IntValue;
    std::uint64_t UIntValue;
    double FloatValue;
  } m_Value;
  BasicType::TypeKind m_TypeKind;
  SourceRange m_Range;
};

class StringLiteralExpr : public Expr {
public:
  StringLiteralExpr(sona::string_ref const& strValue, SourceRange const& range)
    : Expr(NodeKind::CNK_StringLiteralExpr),
      m_StrValue(strValue), m_Range(range) {}

  sona::string_ref const& GetValue() const noexcept {
    return m_StrValue;
  }

  SourceRange const& GetRange() const noexcept {
    return m_Range;
  }

private:
  sona::string_ref m_StrValue;
  SourceRange m_Range;
};

class IdRefExpr : public Expr {
public:
  IdRefExpr(sona::owner<Identifier> id)
    : Expr(NodeKind::CNK_IdRefExpr), m_Id(std::move(id)) {}

  sona::ref_ptr<Identifier const> GetId() const noexcept {
    return m_Id.borrow();
  }

private:
  sona::owner<Identifier> m_Id;
};

class SizeofExpr : public Expr {
public:
private:

};

class FuncCallExpr : public Expr {
public:
  FuncCallExpr(sona::owner<Expr> &&callee,
               std::vector<sona::owner<Expr>> &&args)
    : Expr(NodeKind::CNK_FuncCallExpr),
      m_Callee(std::move(callee)), m_Args(std::move(args)) {}

  sona::ref_ptr<Expr const> GetCallee() const noexcept {
    return m_Callee.borrow();
  }

  auto GetArgs() const noexcept {
    return sona::linq::from_container(m_Args).
        transform([](sona::owner<Expr> const& e) { return e.borrow(); });
  }

private:
  sona::owner<Expr> m_Callee;
  std::vector<sona::owner<Expr>> m_Args;
};

class ArraySubscriptExpr : public Expr {
public:
  ArraySubscriptExpr(sona::owner<Expr> &&array, sona::owner<Expr> &&index)
    : Expr(NodeKind::CNK_ArraySubscriptExpr),
      m_Array(std::move(array)), m_Index(std::move(index)) {}

  sona::ref_ptr<Expr const> GetArrayPart() const noexcept {
    return m_Array.borrow();
  }

  sona::ref_ptr<Expr const> GetIndexPart() const noexcept {
    return m_Index.borrow();
  }

private:
  sona::owner<Expr> m_Array;
  sona::owner<Expr> m_Index;
};

class MemberAccessExpr : public Expr {
public:
  MemberAccessExpr(sona::owner<Syntax::Expr> &&baseExpr,
                   sona::owner<Syntax::Identifier> &&member)
    : Expr(Node::NodeKind::CNK_MemberAccessExpr),
      m_BaseExpr(std::move(baseExpr)), m_Member(std::move(member)) {}

  sona::ref_ptr<Syntax::Expr const> GetBaseExpr() const noexcept {
    return m_BaseExpr.borrow();
  }

  sona::ref_ptr<Syntax::Identifier const> GetMember() const noexcept {
    return m_Member.borrow();
  }

private:
  sona::owner<Syntax::Expr> m_BaseExpr;
  sona::owner<Syntax::Identifier> m_Member;
};

class TransUnit : public Node {
public:
  TransUnit() : Node(NodeKind::CNK_TransUnit) {}

  void Declare(sona::owner<Decl> &&decl) {
    m_Decls.push_back(std::move(decl));
  }

  void DoImport(sona::owner<Import> &&import) {
    m_Imports.push_back(std::move(import));
  }

  auto GetDecls() const noexcept {
    return sona::linq::from_container(m_Decls).
        transform([](sona::owner<Decl> const& decl)
          { return decl.borrow(); } );
  }

  auto GetImports() const noexcept {
    return sona::linq::from_container(m_Imports).
        transform([](sona::owner<Import> const& decl)
          { return decl.borrow(); } );
  }

private:
  std::vector<sona::owner<Decl>> m_Decls;
  std::vector<sona::owner<Import>> m_Imports;
};

} // namespace Syntax;
} // namespace ckx

#endif // CST_H
