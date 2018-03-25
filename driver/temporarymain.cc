#include "sona/log.hpp"
#include "sona/backtrace.hpp"
#include "sona/linq.hpp"

#include <vector>
#include <iostream>

int main() {
    using namespace std;
    using namespace sona;

    vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for (int x : linq::from_container(vec).
                     filter1([](int x) -> bool { return x % 2 == 0; }).
                     reverse().
                     transform([](int x) -> int { return x * x; }).
                     concat_with(linq::from_container(vec)).
                     transform([](int x) -> int { return x * 3 + 1; })) {
        cout << x << ' ';
    }
}
