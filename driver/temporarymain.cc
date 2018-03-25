#include "sona/log.hpp"
#include "sona/backtrace.hpp"

#include "sona/util.hpp"
#include "sona/optional.hpp"
#include "sona/linq.hpp"
#include <iostream>

int main() {
    int a;
    sona::construct(&a, 5);

    sona::log(2333);

    sona_mark_stack1();
    {
        sona_mark_stack1();
        sona_backtrace();
    }

    using namespace std;
    cout << a << endl;

    sona::destroy_at(&a);

    sona::optional<int> b(19);
    cout << b.value() << endl;
}
