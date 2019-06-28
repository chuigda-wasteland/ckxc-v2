#include "sona/log.h"
#include <cstdio>

namespace sona {
namespace log_impl {

using namespace std;

void log(const std::string &str) {
  fprintf(stderr, "sona-log> %s\n", str.c_str());
  fflush(stdout);
}

} // namespace log_impl
} // namespace sona
