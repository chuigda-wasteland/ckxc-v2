#ifndef LOG_H
#define LOG_H

#include <memory>
#include <string>

namespace sona {
namespace log_impl {

void log(std::string const &str);

/*
std::string to_string(char const* str) {
    return std::string(str);
}
*/

template <typename T, typename... Args>
void log(std::string &&str, T &&t, Args &&... args) {
  using namespace std;
  log(move(str) + to_string(forward<T>(t)), forward<Args>(args)...);
}

} // namespace log_impl

template <typename T, typename... Args> void log(T &&value, Args &&... args) {
#ifndef SONA_NO_DEBUG
  log_impl::log(std::string(""), std::forward<T>(value),
                std::forward<Args>(args)...);
#endif
}

} // namespace sona

#endif // LOG_H
