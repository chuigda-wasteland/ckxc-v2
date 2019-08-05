#ifndef REPLINTERPRETER_H
#define REPLINTERPRETER_H

#include "Sema/SemaPhase0.h"
#include "Sema/SemaPhase1.h"
#include "Backend/ASTVisitor.h"

#include "sona/optional.h"
#include <string>

namespace ckx {
namespace Backend {

class ReplValue {
public:
  ReplValue() = default;

  ReplValue(int64_t intValue) { m_Value.IntValue = intValue; }

  ReplValue(uint64_t uIntValue) { m_Value.UIntValue = uIntValue; }

  ReplValue(double floatValue) { m_Value.FloatValue = floatValue; }

  ReplValue(bool boolValue) { m_Value.BoolValue = boolValue; }

  ReplValue(char charValue) { m_Value.CharValue = charValue; }

  ReplValue(sona::ref_ptr<ReplValue> ptrValue) {
    m_Value.PtrValue = ptrValue.operator->();
  }

  void SetIntValue(int64_t intValue) noexcept {
    m_Value.IntValue = intValue;
  }

  void SetUIntValue(uint64_t uIntValue) noexcept {
    m_Value.IntValue = uIntValue;
  }

  void SetFloatValue(double floatValue) noexcept {
    m_Value.FloatValue = floatValue;
  }

  void SetCharValue(char charValue) noexcept {
    m_Value.CharValue = charValue;
  }

  void SetBoolValue(bool boolValue) noexcept {
    m_Value.BoolValue = boolValue;
  }

  void SetPtrValue(sona::ref_ptr<ReplValue> ptrValue) noexcept {
    m_Value.PtrValue = ptrValue.operator->();
  }

  int64_t GetIntValue() const noexcept { return m_Value.IntValue; }

  uint64_t GetUIntValue() const noexcept { return m_Value.UIntValue; }

  double GetFloatValue() const noexcept { return m_Value.FloatValue; }

  char GetCharValue() const noexcept { return m_Value.CharValue; }

  bool GetBoolValue() const noexcept { return m_Value.BoolValue; }

  sona::ref_ptr<ReplValue> GetPtrValue() noexcept {
    return m_Value.PtrValue;
  }

  sona::ref_ptr<ReplValue const> GetPtrValue() const noexcept {
    return m_Value.PtrValue;
  }

private:
  union {
    int64_t IntValue;
    uint64_t UIntValue;
    double FloatValue;
    char CharValue;
    bool BoolValue;
    ReplValue *PtrValue;
  } m_Value;
};

class ReplInterpreter final : public ExprVisitor {
public:
#define AST_EXPR(name) \
  sona::owner<Backend::ActionResult> \
  Visit##name(sona::ref_ptr<AST::name const> expr) override;
#include "AST/Nodes.def"

  void DefineVar(sona::ref_ptr<AST::VarDecl const> decl);

private:
  std::unordered_map<sona::ref_ptr<AST::VarDecl const>, ReplValue> m_Values;
};

} // namespace Backend
} // namespace ckx

#endif // REPLINTERPRETER_H
