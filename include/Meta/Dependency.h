#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <type_traits>
#include <unordered_map>

#include <sona/pointer_plus.hpp>
#include <sona/optional.hpp>

namespace ckx {
namespace Meta {

class DependInfo {
public:
  DependInfo(std::string &&exportedName,
             std::vector<std::string> &&requiredNames)
    : m_ExportedName(std::move(exportedName)),
      m_RequiredNames(std::move(requiredNames)),
      m_ExtraData(nullptr) {}

  std::string const& GetExportedName() const noexcept {
    return m_ExportedName;
  }

  std::vector<std::string> const& GetRequiredNames() const noexcept {
    return m_RequiredNames;
  }

  template <typename ExtraDataType>
  void
  SetExtraData(sona::ref_ptr<ExtraDataType const> extraData) const noexcept {
    m_ExtraData = extraData.get();
  }

  template <typename ExtraDataType>
  sona::ref_ptr<ExtraDataType const> GetExtraDataUnsafe() const noexcept {
    return (ExtraDataType const*)m_ExtraData;
  }

private:
  std::string m_ExportedName;
  std::vector<std::string> m_RequiredNames;
  void const* m_ExtraData;
};

sona::optional<std::vector<sona::ref_ptr<DependInfo>>>
ResolveDependency(std::vector<sona::ref_ptr<DependInfo>> const& dependInfos);

} // namespace Meta
} // namespace ckx

#endif // DEPENDENCY_H
