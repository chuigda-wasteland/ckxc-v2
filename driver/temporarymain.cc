#include "sona/log.hpp"
#include "sona/backtrace.hpp"
#include "sona/linq.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

int main() {
    using namespace std;
    using namespace sona;

    vector<int> vec1 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector<int> vec2 {2, 1, 4, 7, 4, 8, 3, 6, 4, 7};

    for (int x : linq::numeric_range<int>(1, 11).
                     filter1([](int x) -> bool { return x % 2 == 0; }).
                     reverse().
                     transform([](int x) -> int { return x * x; }).
                     concat_with(linq::from_container(vec1)).
                     concat_with(linq::from_container(vec2).slice(2)).
                     transform([](int x) -> int { return x * 3 + 1; })) {
        cout << x << ' ';
    }
}
