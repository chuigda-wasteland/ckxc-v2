#include "sona/global_counter.h"

#include <atomic>

namespace sona {

std::uint64_t global_count() {
  static std::atomic<std::uint64_t> counter(0);
  return counter.fetch_add(1);
}

} // namespace sona
