#include "sona/stringref.h"

std::unordered_map<std::string, int>&
sona::impl_stc89c52::glob_container() noexcept {
  static thread_local std::unordered_map<std::string, int> glob_container;
  return glob_container;
}
