#ifndef TYPEBASE_H
#define TYPEBASE_H

#include "Basic/SourceRange.h"
#include "AST/TypeFwd.h"
#include "Backend/ASTVisitor.h"

#include "sona/ptr_int_pair.h"
#include "sona/stringref.h"

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

/// @note QualType itself can be safely treat as a pointer, so there is no need
/// of ref_ptr + owner
class QualType {
public:
  enum QualId : unsigned {
    QUAL_Const = 0x01, QUAL_Volatile = 0x02, QUAL_Restrict = 0x04
  };

  QualType(sona::ref_ptr<Type const> type)
    : m_PtrIntPair(type.operator->()) {}

  sona::ref_ptr<Type const> GetUnqualTy() const noexcept {
    return m_PtrIntPair.operator->();
  }

  bool IsConst() const noexcept {
    return m_PtrIntPair.get_value() & QUAL_Const;
  }

  bool IsVolatile() const noexcept {
    return m_PtrIntPair.get_value() & QUAL_Volatile;
  }

  bool IsRestrict() const noexcept {
    return m_PtrIntPair.get_value() & QUAL_Restrict;
  }

  void AddConst() noexcept {
    m_PtrIntPair.set_value(m_PtrIntPair.get_value() | QUAL_Const);
  }

  void AddVolatile() noexcept {
    m_PtrIntPair.set_value(m_PtrIntPair.get_value() | QUAL_Volatile);
  }

  void AddRestrict() noexcept {
    m_PtrIntPair.set_value(m_PtrIntPair.get_value() | QUAL_Restrict);
  }

  unsigned GetCVR() const noexcept {
    return m_PtrIntPair.get_value();
  }

  void SetCVR(unsigned value) noexcept {
    sona_assert(value <= (QUAL_Const + QUAL_Volatile + QUAL_Restrict));
    m_PtrIntPair.set_value(value);
  }

  bool EqualTo(QualType that) const noexcept {
    return GetCVR() == that.GetCVR()
        && GetUnqualTy()->EqualTo(that.GetUnqualTy().get());
  }

private:
  sona::ptr_int_pair<AST::Type const, 3> m_PtrIntPair;
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

#endif // TYPEBASE_H
