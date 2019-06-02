#ifndef TYPEBASE_HPP
#define TYPEBASE_HPP

#include "Basic/SourceRange.hpp"
#include "AST/TypeFwd.hpp"
#include "Backend/ASTVisitor.h"

#include "sona/stringref.hpp"

#include <cstdint>
#include <type_traits>

namespace ckx {
namespace AST {

class Type {
public:
  enum class TypeId : std::int8_t {
    /// Basic / Builtin
    TI_Builtin,

    /// Composed
    TI_Tuple,
    TI_Array,
    TI_Pointer,
    TI_Ref,
    TI_Function,

    /// User-defined
    TI_UserDefined,

    /// @todo where should we add "Qualifiers"?
  };

  TypeId GetTypeId() const { return m_Id; }

  virtual std::size_t GetHash() const noexcept = 0;
  virtual bool EqualTo(Type const &that) const noexcept = 0;

  virtual sona::owner<Backend::ActionResult>
  Accept(sona::ref_ptr<Backend::TypeVisitor> visitor) const = 0;

  virtual ~Type() = default;

protected:
  Type(TypeId id) : m_Id(id) {}

private:
  TypeId m_Id;
};

struct TypeEqual {
  bool operator()(Type const &T1, Type const &T2) const noexcept {
    return T1.EqualTo(T2);
  }
};

template <typename Type_t> struct TypeHash {
  std::size_t operator()(Type_t const &type) const noexcept {
    static_assert(std::is_base_of<Type, Type_t>::value, "");
    return type.GetHash();
  }
};

} // namespace AST
} // namespace ckx

#endif // TYPEBASE_HPP
