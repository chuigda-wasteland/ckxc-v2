#include "Syntax/CST.h"
#include <algorithm>

using namespace ckx;
using namespace Syntax;
using namespace std;
using namespace sona;

CSTNode::~CSTNode() {}

Meta::DependInfo
CSTClassDecl::
CompileDependency(vector<string_ref> const &importedNames) const noexcept {
  vector<string_ref> requirements;
  for (sona::ref_ptr<CSTDecl const> decl : GetSubDecls()) {
    Meta::DependInfo subDep = decl.get().CompileDependency(importedNames);
    for (string_ref str : subDep.GetRequiredNames()) {
      if (str == GetClassName().GetIdentifier()
          || find(importedNames.cbegin(), importedNames.cend(), str)
             == importedNames.cend()) {
        continue;
      }
      requirements.push_back(str);
    }
  }

  return Meta::DependInfo(GetClassName().GetIdentifier(),
                          std::move(requirements), this);
}
