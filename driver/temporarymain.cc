#include "sona/log.hpp"
#include "sona/backtrace.hpp"

int main() {
    sona::log(2333);

    sona_mark_stack1();
    {
        sona_mark_stack1();
        sona_backtrace();
    }
}
