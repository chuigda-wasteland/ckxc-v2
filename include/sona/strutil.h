#ifndef STRUTIL_H
#define STRUTIL_H

#include <string>
#include <vector>

namespace sona {

void str_split(std::string const& source, char splitter,
               std::vector<std::string> &result);

template <typename SplitCondition /* SplitCondition(current, peeked) */ >
void str_split(std::string const& source, SplitCondition condition,
               std::vector<std::string> &result) {
  size_t idx = 0;
  std::string current;
  while (idx < source.size()) {
    if (condition(source[idx], source[idx+1])) {
      result.push_back(std::move(current));
      ++idx;
      continue;
    }
    current.push_back(source[idx]);
    ++idx;
  }
}

} // sona

#endif // STRUTIL_H
