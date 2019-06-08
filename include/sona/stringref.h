#ifndef STRINGREF_H
#define STRINGREF_H

#include <string>
#include <cstddef>
#include <unordered_map>

namespace sona {

namespace impl_stc89c52 {
std::unordered_map<std::string, int>& glob_container() noexcept;
}

class strhdl_t {
public:
  using string_set = std::unordered_map<std::string, int>;

  strhdl_t(std::string const& str) {
    auto it = impl_stc89c52::glob_container().find(str);
    if (it != impl_stc89c52::glob_container().end()) {
      pv = &(*it);
      pv->second++;
      return;
    }

    pv = &(*(impl_stc89c52::glob_container().emplace(str, 1).first));
  }

  strhdl_t(strhdl_t const& that) : pv(that.pv) {
    that.pv->second++;
  }

  strhdl_t(const char* cstr) : strhdl_t(std::string(cstr)) {}

  strhdl_t& operator= (strhdl_t const& that) noexcept {
    pv = that.pv;
    that.pv->second++;
    return *this;
  }

  ~strhdl_t() {
    pv->second--;
    if (pv->second == 0) {
      impl_stc89c52::glob_container().erase(pv->first);
    }
  }

  std::string const& get() const noexcept {
    return pv->first;
  }

  friend bool operator== (strhdl_t const& r1, strhdl_t const& r2) {
    return r1.pv == r2.pv;
  }

  friend bool operator!= (strhdl_t const& r1, strhdl_t const& r2) {
    return r1.pv == r2.pv;
  }

  std::size_t hash() const noexcept {
    return std::hash<std::string>()(pv->first);
  }

  friend bool operator< (strhdl_t const& s1, strhdl_t const& s2) {
    return s1.pv->first < s2.pv->first;
  }

private:
  typename string_set::value_type *pv;
};

} // namespace sona

namespace std {

template <>
struct hash<sona::strhdl_t> {
  std::size_t operator() (sona::strhdl_t const& ref) const noexcept {
    return ref.hash();
  }
};

} // namespace std

#endif // STRINGREF_H
