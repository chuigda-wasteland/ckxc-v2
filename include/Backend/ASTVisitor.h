#ifndef ASTVISITOR_H
#define ASTVISITOR_H

#include "AST/DeclFwd.h"
#include "AST/StmtFwd.h"
#include "AST/ExprFwd.h"
#include "AST/TypeFwd.h"

#include "sona/pointer_plus.h"
#include "sona/optional.h"
#include "sona/util.h"

namespace ckx {
namespace Backend {

template <typename T> class ActionResultImpl;

class ActionResult {
public:
  virtual ~ActionResult() = 0;

  template <typename T> T const& GetValue() const noexcept {
    ActionResultImpl<T> const* impl =
        static_cast<ActionResultImpl<T> const*>(this);
    return impl->GetValue();
  }

  template <typename T> sona::optional<T> GetValueChecked() const noexcept {
    ActionResultImpl<T> *impl = dynamic_cast<ActionResultImpl<T>*>(this);
    if (impl == nullptr) {
      return sona::empty_optional();
    }
    else {
      return impl->GetValue();
    }
  }
};

template <typename T>
class ActionResultImpl : public ActionResult {
public:
  ActionResultImpl(T&& value) : m_Value(std::forward<T>(value)) {}
  ActionResultImpl(ActionResultImpl const&) = delete;
  ActionResultImpl(ActionResultImpl&&) = delete;
  ActionResultImpl& operator=(ActionResultImpl const&) = delete;
  ActionResultImpl& operator=(ActionResultImpl &&) = delete;

  T const& GetValue() const noexcept { return m_Value; }

private:
  T m_Value;
};

class VoidType {};

template<>
class ActionResultImpl<VoidType> : public ActionResult {
public:
  ActionResultImpl(VoidType v) { (void)v; }
  ActionResultImpl(ActionResultImpl const&) = delete;
  ActionResultImpl(ActionResultImpl&&) = delete;
  ActionResultImpl& operator=(ActionResultImpl const&) = delete;
  ActionResultImpl& operator=(ActionResultImpl &&) = delete;

  void* GetValue() const noexcept { return nullptr; }
};

template <typename T> sona::owner<ActionResult> CreateResult(T&& t) {
  return new ActionResultImpl<T>(std::forward<T>(t));
}

class DeclVisitor {
public:
#define AST_DECL(name) \
  virtual sona::owner<Backend::ActionResult> \
  Visit##name(sona::ref_ptr<AST::name const> decl) = 0;
#include "AST/Nodes.def"
};

class TypeVisitor {
public:
#define AST_TYPE(name) \
  virtual sona::owner<Backend::ActionResult> \
  Visit##name(sona::ref_ptr<AST::name const> type) = 0;
#include "AST/Nodes.def"
};

class ExprVisitor {
public:
#define AST_EXPR(name) \
  virtual sona::owner<Backend::ActionResult> \
  Visit##name(sona::ref_ptr<AST::name const> expr) = 0;
#include "AST/Nodes.def"
};

class StmtVisitor {
public:
#define AST_STMT(name) \
  virtual sona::owner<Backend::ActionResult> \
  Visit##name(sona::ref_ptr<AST::name const> stmt) = 0;
#include "AST/Nodes.def"
};

} // namespace Backend
} // namespace ckx

#endif // ASTVISITOR_H
