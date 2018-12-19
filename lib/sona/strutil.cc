#include "sona/strutil.h"

namespace sona {

void str_split(std::string const& source, char splitter,
               std::vector<std::string> &result) {
  return str_split(source,
                   [&splitter](char cur, char) { return cur == splitter; },
                   result);
}

} // namespace sona
