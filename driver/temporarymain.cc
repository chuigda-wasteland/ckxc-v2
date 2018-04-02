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

    small_vector<int, 4> vec {1, 2, 3, 4};
    small_vector<int, 4> vec1 {1, 2, 3, 4, 5};

    for (int x : linq::from_container(vec).
                     transform([](int v) { return v*3 + 1; })) {
        cout << x << ' ';
    }
    cout << endl;

    for (int x : linq::from_container(vec1).
                     transform([](int v) { return v*3 + 1; })) {
        cout << x << ' ';
    }
    cout << endl;
}
