#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <stack>
#include <type_traits>
#include <unordered_map>

#include <sona/pointer_plus.hpp>
#include <sona/optional.hpp>
#include <sona/stringref.hpp>

namespace ckx {
namespace Meta {

class DependInfo {
public:
  DependInfo(sona::string_ref exportedName,
             std::vector<sona::string_ref> &&requiredNames,
             void const* extraData)
    : m_ExportedName(exportedName),
      m_RequiredNames(requiredNames),
      m_ExtraData(extraData) {}

  DependInfo(sona::string_ref exportedName,
             std::vector<sona::string_ref> &&requiredNames)
    : DependInfo(exportedName, std::move(requiredNames), nullptr) {}

  sona::string_ref GetExportedName() const noexcept {
    return m_ExportedName;
  }

  std::vector<sona::string_ref> const& GetRequiredNames() const noexcept {
    return m_RequiredNames;
  }

  template <typename ExtraDataType>
  void
  SetExtraData(sona::ref_ptr<ExtraDataType const> extraData) const noexcept {
    m_ExtraData = extraData.get();
  }

  template <typename ExtraDataType>
  sona::ref_ptr<ExtraDataType const> GetExtraDataUnsafe() const noexcept {
    return reinterpret_cast<ExtraDataType const*>(m_ExtraData);
  }

private:
  sona::string_ref m_ExportedName;
  std::vector<sona::string_ref> m_RequiredNames;
  void const* m_ExtraData;
};

class DependContext {
public:

private:
  std::unordered_map<std::string, DependInfo> m_CollectedDependInfo;
  std::stack<std::vector<sona::string_ref>> m_KnownSymbols;
  std::stack<sona::string_ref> m_EnclosingScopeSymbol;
};

sona::optional<std::vector<sona::ref_ptr<DependInfo>>>
ResolveDependency(std::vector<sona::ref_ptr<DependInfo>> const& dependInfos);

} // namespace Meta
} // namespace ckx

#endif // DEPENDENCY_H
