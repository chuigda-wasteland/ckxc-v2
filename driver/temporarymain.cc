#include "sona/log.hpp"
#include "sona/backtrace.hpp"
#include "sona/linq.hpp"
#include "sona/small_vector.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

int main() {
    using namespace std;
    using namespace sona;

    small_vector<int, 4> vec;
    vec.push_back(4);
    vec.push_back(3);
    vec.push_back(2);
    vec.push_back(1);
    vec.push_back(9);

    for (int x : linq::from_container(vec).
                     transform([](int v) { return v*3 + 1; })) {
        cout << x << ' ';
    }
}
