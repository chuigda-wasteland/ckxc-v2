#include "Sema/Sema.h"

#include "Syntax/CST.h"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Expr.hpp"
#include "AST/Type.hpp"

using namespace ckx;
using namespace ckx::Sema;
using namespace sona;

void SemaClass::PushScope(Scope::ScopeFlags flags) {
  m_ScopeChains.emplace_back(GetCurrentScope(), flags);
}

void SemaClass::PopScope() {
  m_ScopeChains.pop_back();
}

ref_ptr<AST::Type const>
SemaClass::ResolveType(ref_ptr<Syntax::Type const> type) {
  switch (type->GetNodeKind()) {
#define CST_TYPE(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return Resolve##name(static_cast<Syntax::name const&>(type.get()));
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
    return nullptr;
  }
}

ref_ptr<AST::Decl>
SemaClass::ActOnDecl(ref_ptr<Syntax::Decl const> decl) {
  switch (decl->GetNodeKind()) {
#define CST_DECL(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return ActOn##name(static_cast<Syntax::name const&>(decl.get()));
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
    return nullptr;
  }
}

/*

owner<AST::Stmt>
SemaClass::ActOnStmt(ref_ptr<Syntax::Stmt const> stmt) {
  switch (stmt->GetNodeKind()) {
#define CST_STMT(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return ActOn##name(static_cast<Syntax::name const&>(stmt.get()));
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
    return nullptr;
  }
}

owner<AST::Expr>
SemaClass::ActOnExpr(ref_ptr<Syntax::Expr const> expr) {
  switch (expr->GetNodeKind()) {
#define CST_EXPR(name) \
  case Syntax::Node::NodeKind::CNK_##name: \
    return ActOn##name(static_cast<Syntax::name const&>(expr.get()));
#include "Syntax/CSTNodeDefs.def"
  default:
    sona_unreachable();
    return nullptr;
  }
}

*/

ref_ptr<AST::Type const>
SemaClass::ResolveBasicType(ref_ptr<Syntax::BasicType const> type) {
  AST::BuiltinType::BuiltinTypeId btid;
  switch (type->GetTypeKind()) {
  case Syntax::BasicType::TypeKind::TK_Int8:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_i8; break;
  case Syntax::BasicType::TypeKind::TK_Int16:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_i16; break;
  case Syntax::BasicType::TypeKind::TK_Int32:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_i32; break;
  case Syntax::BasicType::TypeKind::TK_Int64:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_i64; break;
  case Syntax::BasicType::TypeKind::TK_UInt8:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_u8; break;
  case Syntax::BasicType::TypeKind::TK_UInt16:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_u16; break;
  case Syntax::BasicType::TypeKind::TK_UInt32:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_u32; break;
  case Syntax::BasicType::TypeKind::TK_UInt64:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_u64; break;
  case Syntax::BasicType::TypeKind::TK_Float:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_r32; break;
  case Syntax::BasicType::TypeKind::TK_Double:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_r64; break;
  case Syntax::BasicType::TypeKind::TK_Quad:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_r128; break;
  case Syntax::BasicType::TypeKind::TK_Bool:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_bool; break;
  case Syntax::BasicType::TypeKind::TK_Void:
    btid = AST::BuiltinType::BuiltinTypeId::BTI_void; break;
  default:
    sona_unreachable1("unhandled case");
  }

  return m_ASTContext.GetBuiltinType(btid);
}

ref_ptr<AST::Type const>
SemaClass::ResolveUserDefinedType(ref_ptr<Syntax::UserDefinedType const> type) {
  return m_ScopeChains.back().LookupType(type->GetName());
}

ref_ptr<AST::Type const>
SemaClass::ResolveTemplatedType(ref_ptr<Syntax::TemplatedType const> type) {
  (void)type;
  sona_unreachable1("not implemented");
  return nullptr;
}

ref_ptr<AST::Type const>
SemaClass::ResolveComposedType(ref_ptr<Syntax::ComposedType const> type) {
  ref_ptr<Syntax::Type const> rootType = type->GetRootType();
  ref_ptr<AST::Type const> resolvedRootType = ResolveType(rootType);

  if (resolvedRootType == nullptr) {
    return nullptr;
  }

  for (Syntax::ComposedType::TypeSpecifier tySpec
       : type->GetTypeSpecifiers()) {
    /// @todo add proper diagnostics
    switch (tySpec) {
    case Syntax::ComposedType::TypeSpecifier::CTS_Const:
    case Syntax::ComposedType::TypeSpecifier::CTS_Volatile:
      sona_unreachable1("not implemented");
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_Pointer:
      resolvedRootType =
          m_ASTContext.CreatePointerType(resolvedRootType).get();
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_Ref:
      resolvedRootType =
          m_ASTContext.CreateLValueRefType(resolvedRootType).get();
      break;
    case Syntax::ComposedType::TypeSpecifier::CTS_RvRef:
      resolvedRootType =
          m_ASTContext.CreateRValueRefType(resolvedRootType).get();
      break;
    }
  }

  return resolvedRootType;
}

ref_ptr<AST::Decl>
SemaClass::ActOnClassDecl(sona::ref_ptr<Syntax::ClassDecl const> decl) {
  if (GetCurrentScope()->LookupType(decl->GetClassName()) != nullptr) {
    /// @todo report error
    return nullptr;
  }

  AST::ClassDecl *classDecl = new AST::ClassDecl(GetCurrentDeclContext(),
                                                 decl->GetClassName());
  owner<AST::Decl> classDeclOwner(classDecl);
  PushDeclContext(classDecl);
  PushScope(Scope::ScopeFlags::SF_Class);

  for (ref_ptr<Syntax::Decl const> subDecl : decl->GetSubDecls()) {
    ActOnDecl(subDecl);
  }

  PopScope();
  PopDeclContext();
  GetCurrentDeclContext()->AddDecl(std::move(classDecl));

  AST::ClassType *classType =
      new AST::ClassType(decl->GetClassName(),
                         ref_ptr<AST::ClassDecl>(classDecl));
  m_ASTContext.AddUserDefinedType(owner<AST::Type>(classType));
  GetCurrentScope()->AddType(decl->GetClassName(),
                             ref_ptr<AST::Type>(classType));

  return ref_ptr<AST::Decl>(classDecl);
}

ref_ptr<AST::Decl>
SemaClass::ActOnEnumDecl(ref_ptr<Syntax::EnumDecl const> decl) {
  if (GetCurrentScope()->LookupType(decl->GetName()) != nullptr) {
    /// @todo report error
    return nullptr;
  }

  AST::EnumDecl *enumDecl = new AST::EnumDecl(GetCurrentDeclContext(),
                                              decl->GetName());
  owner<AST::Decl> enumDeclOwner(enumDecl);
  PushDeclContext(enumDecl);
  PushScope(Scope::ScopeFlags::SF_Enum);

  int64_t currentEnumValue = 0;
  for (Syntax::EnumDecl::Enumerator const& enumerator
       : decl->GetEnumerators()) {
    if (enumerator.HasValue()) {
      currentEnumValue = enumerator.GetValueUnsafe();
    }

    AST::EnumeratorDecl *enumeratorDecl =
        new AST::EnumeratorDecl(GetCurrentDeclContext(),
                                enumerator.GetName(),
                                currentEnumValue);
    GetCurrentDeclContext()->AddDecl(owner<AST::Decl>(enumeratorDecl));
    currentEnumValue++;
  }

  PopScope();
  PopDeclContext();
  GetCurrentDeclContext()->AddDecl(std::move(enumDeclOwner));

  AST::EnumType *enumType =
      new AST::EnumType(decl->GetName(),
                        ref_ptr<AST::EnumDecl>(enumDecl));
  m_ASTContext.AddUserDefinedType(owner<AST::Type>(enumType));
  GetCurrentScope()->AddType(decl->GetName(),
                             ref_ptr<AST::Type>(enumType));
  return ref_ptr<AST::Decl>(enumDecl);
}

ref_ptr<AST::Decl>
SemaClass::ActOnVarDecl(ref_ptr<Syntax::VarDecl const> decl) {
  if (GetCurrentScope()->LookupVarDecl(decl->GetName()) != nullptr) {
    /// @todo report error
    return nullptr;
  }

  ref_ptr<AST::Type const> varType = ResolveType(decl->GetType());
  if (varType == nullptr) {
    return nullptr;
  }

  AST::VarDecl *varDecl = new AST::VarDecl(GetCurrentDeclContext(),
                                           varType,
                                           AST::DeclSpec::DS_None,
                                           decl->GetName());
  GetCurrentDeclContext()->AddDecl(owner<AST::Decl>(varDecl));
  GetCurrentScope()->AddVarDecl(ref_ptr<AST::VarDecl const>(varDecl));
  return ref_ptr<AST::Decl>(varDecl);
}

ref_ptr<AST::Decl>
SemaClass::ActOnFuncDecl(ref_ptr<Syntax::FuncDecl const> decl) {
  ref_ptr<AST::Type const> retType = ResolveType(decl->GetReturnType());
  if (retType == nullptr) {
    return nullptr;
  }

  std::vector<ref_ptr<AST::Type const>> paramTypes;
  for (ref_ptr<Syntax::Type const> concreteParamType : decl->GetParamTypes()) {
    ref_ptr<AST::Type const> paramType = ResolveType(concreteParamType);
    if (paramType != nullptr) {
      paramTypes.push_back(paramType);
    }
  }

  ref_ptr<AST::FunctionType const> funcType =
      m_ASTContext.BuildFunctionType(paramTypes, retType);

  for (ref_ptr<AST::FuncDecl const> existingFunc :
       sona::linq::from_container(
         GetCurrentScope()->GetAllFuncs(decl->GetName())).
           transform([](const auto& p) { return p.second; })) {
    ref_ptr<AST::FunctionType const> existingFuncType =
        m_ASTContext.BuildFunctionType(existingFunc->GetParamTypes(),
                                       existingFunc->GetRetType());
    if (existingFuncType == funcType) {
      /// @todo check redeclaration, redefinition and more
    }
  }

  /// @todo implement the rest part

  return nullptr;
}
