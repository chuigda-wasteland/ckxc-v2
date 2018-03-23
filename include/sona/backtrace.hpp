#ifndef BACKTRACE_HPP
#define BACKTRACE_HPP

#ifndef SONA_NO_DEBUG

namespace sona {
namespace backtrace_impl {

void create_mark(char const* file, int line,
                char const* func, char const* desc);
void remove_last_mark();
void print_backtrace();

class marker {
public:
    marker(char const* file, int line, char const* func, char const* desc) {
        backtrace_impl::create_mark(file, line, func, desc);
    }

    ~marker() {
        backtrace_impl::remove_last_mark();
    }

    marker(marker const&) = delete;
    marker(marker&&) = delete;

    marker& operator= (const marker&) = delete;
    marker& operator= (marker&&) = delete;
};

} // namespace backtrace
} // namespace sona

#define sona_mark_stack(desc) sona::backtrace_impl::marker \
    ___sona_builtin_marker_var(__FILE__, __LINE__, __func__, desc);
#define sona_mark_stack1() sona::backtrace_impl::marker \
    __sona_builtin_marker_var(__FILE__, __LINE__, __func__, "");
#define sona_backtrace() sona::backtrace_impl::print_backtrace();

#else

#define sona_mark_stack(desc) {;}
#define sona_mark_stack1() {;}
#define sona_backtrace() {;}

#endif

#endif // BACKTRACE_HPP
