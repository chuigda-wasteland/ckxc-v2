#include "VKTestCXX.h"
#include "AST/ASTContext.h"

#include "sona/linq.h"

#include <iostream>
#include <string>

using namespace sona;
using namespace ckx;
using namespace std;

void test0() {
  VkTestSectionStart("Fetching singletons of builtin types");

  AST::ASTContext context;
  std::vector<AST::QualType> turn1, turn2;

#define BUILTIN_TYPE(name, size, ii, is, sd, usd, tk) \
  { \
    AST::QualType ty = \
      context.GetBuiltinType(AST::BuiltinType::BTI_##name); \
    VkAssertEquals(AST::Type::TypeId::TI_Builtin, \
                   ty.GetUnqualTy()->GetTypeId()); \
    sona::ref_ptr<AST::BuiltinType const> bty = \
            ty.GetUnqualTy().cast_unsafe<AST::BuiltinType const>(); \
    VkAssertEquals(AST::BuiltinType::BTI_##name, bty->GetBtid()); \
    turn1.push_back(ty); \
  }
#include "Syntax/BuiltinTypes.def"

#define BUILTIN_TYPE(name, size, ii, is, sd, usd, tk) \
  { \
    AST::QualType ty = \
      context.GetBuiltinType(AST::BuiltinType::BTI_##name); \
    turn2.push_back(ty); \
  }
#include "Syntax/BuiltinTypes.def"

  auto r = linq::from_container(turn1).zip_with(linq::from_container(turn2));
  for (const auto &p : r) { VkAssertEquals(p.first, p.second); }
}

void test1() {
  VkTestSectionStart("Creating singletons of ptr/ref/arr types");

  AST::ASTContext context;
  std::vector<AST::QualType> turn1, turn2;

#define BUILTIN_TYPE(name, size, ii, is, sd, usd, tk) \
  { \
    AST::QualType ty = \
      context.GetBuiltinType(AST::BuiltinType::BTI_##name); \
    AST::QualType pty  = context.CreatePointerType(ty); \
    AST::QualType lrty = context.CreateLValueRefType(ty); \
    AST::QualType rrty = context.CreateRValueRefType(ty); \
    AST::QualType aty  = context.CreateArrayType(ty, 42); \
    turn1.push_back(pty); \
    turn1.push_back(lrty); \
    turn1.push_back(rrty); \
    turn1.push_back(aty); \
  }
#include "Syntax/BuiltinTypes.def"

#define BUILTIN_TYPE(name, size, ii, is, sd, usd, tk) \
  { \
    AST::QualType ty = \
      context.GetBuiltinType(AST::BuiltinType::BTI_##name); \
    AST::QualType pty  = context.CreatePointerType(ty); \
    AST::QualType lrty = context.CreateLValueRefType(ty); \
    AST::QualType rrty = context.CreateRValueRefType(ty); \
    AST::QualType aty  = context.CreateArrayType(ty, 42); \
    turn2.push_back(pty); \
    turn2.push_back(lrty); \
    turn2.push_back(rrty); \
    turn2.push_back(aty); \
  }
#include "Syntax/BuiltinTypes.def"

  auto r = linq::from_container(turn1).zip_with(linq::from_container(turn2));
  for (const auto &p : r) { VkAssertEquals(p.first, p.second); }
}

void test2() {
  VkTestSectionStart("Creating singletons of function types");
  AST::ASTContext context;

  std::vector<AST::QualType> paramTypes;
  paramTypes.push_back(
        context.CreatePointerType(
          context.GetBuiltinType(
            AST::BuiltinType::BTI_Int8)));
  paramTypes.push_back(
        context.CreateLValueRefType(
          context.GetBuiltinType(
            AST::BuiltinType::BTI_Char)));
  paramTypes.push_back(
          context.GetBuiltinType(
            AST::BuiltinType::BTI_NilType));
  AST::QualType retType =
        context.GetBuiltinType(
          AST::BuiltinType::BTI_Char);

  std::vector<AST::QualType> paramTypes2 = paramTypes;

  AST::QualType fnty1 =
      context.BuildFunctionType(std::move(paramTypes), retType);
  AST::QualType fnty2 =
      context.BuildFunctionType(std::move(paramTypes), retType);
  VkAssertEquals(fnty1, fnty2);
}

int main() {
  VkTestStart();

  test0();
  test1();
  test2();

  VkTestFinish();
}
