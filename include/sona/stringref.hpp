#ifndef STRINGREF_HPP
#define STRINGREF_HPP

#include <string>
#include <cstddef>
#include <unordered_map>

namespace sona {

namespace impl_stc89c52 {
std::unordered_map<std::string, int> glob_container() noexcept;
}

class string_ref {
public:
  using string_set = std::unordered_map<std::string, int>;

  string_ref(std::string &&str) noexcept {
    pv = &(*(impl_stc89c52::glob_container().emplace(std::move(str), 1).first));
  }

  string_ref(std::string const& str) {
    pv = &(*(impl_stc89c52::glob_container().emplace(str, 1).first));
  }

  string_ref(string_ref const& that) : pv(that.pv) {
    that.pv->second++;
  }

  string_ref(const char* cstr) : string_ref(std::string(cstr)) {}

  string_ref& operator= (string_ref const& that) noexcept {
    pv = that.pv;
    that.pv->second++;
    return *this;
  }

  ~string_ref() {
    pv->second--;
    if (pv->second == 0) {
      impl_stc89c52::glob_container().erase(pv->first);
    }
  }

  std::string const& get() const noexcept {
    return pv->first;
  }

  friend bool operator== (string_ref const& r1, string_ref const& r2) {
    return r1.pv == r2.pv;
  }

  friend bool operator!= (string_ref const& r1, string_ref const& r2) {
    return r1.pv == r2.pv;
  }

private:
  typename string_set::value_type *pv;
};

} // namespace sona

#endif // STRINGREF_HPP
