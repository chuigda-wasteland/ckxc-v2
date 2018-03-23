#include "sona/backtrace.hpp"

#ifndef SONA_NO_DEBUG

#include <list>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <string>

/*
extern "C" int __cxa_thread_atexit(void (*func)(), void *obj,
                                   void *dso_symbol) {
  int __cxa_thread_atexit_impl(void (*)(), void *, void *);
  return __cxa_thread_atexit_impl(func, obj, dso_symbol);
}
*/


namespace sona {
namespace backtrace_impl {

using namespace std;

static void backtrace_signal_handler(int);

class backtrace_keeper {
public:
    static backtrace_keeper& instance() {
        static thread_local backtrace_keeper keeper;
        return keeper;
    }

    void create_mark(char const* file, int line,
                     char const* func, char const* desc) {
        marks.emplace_back(file, line, func, desc);
    }

    void remove_last_mark() {
        marks.pop_back();
    }

    void print_backtrace() {
        for (mark const& one_mark : marks) {
            fprintf(stderr, "In file %s, line %d, func %s : %s\n",
                    one_mark.file.c_str(), one_mark.line,
                    one_mark.func.c_str(), one_mark.desc.c_str());
        }
        fflush(stderr);
    }

private:
    backtrace_keeper() {
        signal(SIGABRT, &backtrace_signal_handler);
        signal(SIGILL, &backtrace_signal_handler);
        signal(SIGSEGV, &backtrace_signal_handler);
    }

    struct mark {
        string const file;
        int const line;
        string const func;
        string const desc;

        mark(char const* file, int line, char const* func, char const* desc) :
            file(file), line(line), func(func), desc(desc) {}
        mark(mark const&) = delete;
        mark& operator= (const mark&) = delete;
    };

    list<mark> marks;

};

void create_mark(char const* file, int line,
                 char const* func, char const* desc) {
    backtrace_keeper::instance().create_mark(file, line, func, desc);
}

void remove_last_mark() {
    backtrace_keeper::instance().remove_last_mark();
}

void print_backtrace() {
    backtrace_keeper::instance().print_backtrace();
}

static void backtrace_signal_handler(int) {
    print_backtrace();
    signal(SIGABRT, SIG_DFL);
    abort();
}

} // namespace backtrace_impl
} // namespace sona

#endif
