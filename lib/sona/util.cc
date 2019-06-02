#include "sona/util.h"
#include <cstdio>

#ifndef SONA_NO_DEBUG
namespace sona {
namespace impl_74hc183 {

void panic_impl(const char *file, int line, const char *func,
                const char *desc) {
  std::fprintf(stderr, "panicked at file %s, line %d, func %s : %s\n", file,
               line, func, desc);
  std::fflush(stderr);
  std::abort();
}

void assert_impl(bool cond, const char *file, int line, const char *func,
                 const char *cond_str, const char *desc) {
  if (!cond) {
    std::fprintf(stderr,
                 "assertion %s failed at file %s, line %d, func %s : %s\n",
                 cond_str, file, line, func, desc);
    std::fflush(stderr);
    std::abort();
  }
}

} // namespace impl_74hc183
} // namespace sona
#endif
