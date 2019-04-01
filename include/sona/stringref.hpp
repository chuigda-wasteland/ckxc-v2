#ifndef STRINGREF_HPP
#define STRINGREF_HPP

#include <string>
#include <cstddef>
#include <unordered_map>

namespace sona {

namespace impl_stc89c52 {
std::unordered_map<std::string, int>& glob_container() noexcept;
}

class string_ref {
public:
  using string_set = std::unordered_map<std::string, int>;

  string_ref(std::string const& str) {
    auto it = impl_stc89c52::glob_container().find(str);
    if (it != impl_stc89c52::glob_container().end()) {
      pv = &(*it);
      pv->second++;
      return;
    }

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

  std::size_t hash() const noexcept {
    return std::hash<std::string>()(pv->first);
  }

  friend bool operator< (string_ref const& s1, string_ref const& s2) {
    return s1.pv->first < s2.pv->first;
  }

private:
  typename string_set::value_type *pv;
};

} // namespace sona

namespace std {

template <>
struct hash<sona::string_ref> {
  std::size_t operator() (sona::string_ref const& ref) const noexcept {
    return ref.hash();
  }
};

} // namespace std

#endif // STRINGREF_HPP
