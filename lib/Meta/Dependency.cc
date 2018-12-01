#include "Meta/Dependency.h"

namespace ckx {
namespace Meta {

class GraphNode {
public:
  void MarkTemporarily() noexcept { m_TemporarilyMarked = true; }
  void MarkPermanently() noexcept { m_PermanentlyMarked = true; }
  bool IsMarkedTemporarily() const noexcept { return m_TemporarilyMarked; }
  bool IsMarkedPermanently() const noexcept { return m_PermanentlyMarked; }
  void SetData(sona::ref_ptr<DependInfo> data) noexcept { m_Data = data; }
  sona::ref_ptr<DependInfo> GetData() const noexcept { return m_Data; }

private:
  sona::ref_ptr<DependInfo> m_Data = nullptr;
  bool m_TemporarilyMarked = false;
  bool m_PermanentlyMarked = false;
};

bool VisitGraphNode(GraphNode &node,
                    std::unordered_map<std::string, GraphNode> &nodeSet,
                    std::vector<sona::ref_ptr<DependInfo>> &sorted) {
  if (node.IsMarkedPermanently()) {
    return true;
  }

  if (node.IsMarkedTemporarily()) {
    return false;
  }

  node.MarkTemporarily();
  for (sona::string_ref const& requiredName
       : node.GetData().get().GetRequiredNames()) {
    auto it = nodeSet.find(requiredName.get());
    if (it == nodeSet.end()) {
      return false;
    }

    if (!VisitGraphNode(it->second, nodeSet, sorted)) {
      return false;
    }
  }

  node.MarkPermanently();
  sorted.push_back(node.GetData());
  return true;
}

sona::optional<std::vector<sona::ref_ptr<DependInfo>>>
ResolveDependency(std::vector<sona::ref_ptr<DependInfo>> const& dependInfos) {
  std::vector<sona::ref_ptr<DependInfo>> sorted;
  if (dependInfos.size() == 0) {
    return sorted;
  }

  std::unordered_map<std::string, GraphNode> nodeSet;
  for (sona::ref_ptr<DependInfo> dependInfo : dependInfos) {
    nodeSet[dependInfo.get().GetExportedName().get()].SetData(dependInfo);
  }

  bool hasUnmarked = true;
  while (hasUnmarked) {
    hasUnmarked = false;
    for (auto &p : nodeSet) {
      GraphNode &node = p.second;
      if (node.IsMarkedTemporarily() || node.IsMarkedPermanently()) {
        continue;
      }
      hasUnmarked = true;
      if (!VisitGraphNode(node, nodeSet, sorted)) {
        return sona::empty_optional();
      }
    }
  }

  return sorted;
}

} // namespace Meta
} // namespace ckx
