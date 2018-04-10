#include "sona/log.hpp"
#include "sona/backtrace.hpp"
#include "sona/linq.hpp"
#include "sona/small_vector.hpp"
#include "sona/q_list.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

int main() {
    using namespace std;
    using namespace sona;

    small_vector<int, 4> vec { 1, 2, 3, 4 };
    small_vector<int, 4> another(std::move(vec));
    q_list<int> list { 9, 9, 3, 4, 1, 7, 2 };

    for (int x : linq::from_container(vec).
                     transform([](int v) { return v*3 + 1; })) {
        cout << x << ' ';
    }
    cout << endl;

    std::sort(list.begin(), list.end());
    for (int x : linq::from_container(list).
                     transform([](int v) { return v*3 + 1; })) {
        cout << x << ' ';
    }
    cout << endl;
}
