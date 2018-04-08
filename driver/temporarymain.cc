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
    q_list<int> list { 1, 2, 3, 4 };
    list.pop_back();
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);
    list.push_back(9);

    for (int x : linq::from_container(vec).
                     transform([](int v) { return v*3 + 1; })) {
        cout << x << ' ';
    }
    cout << endl;

    for (int x : linq::from_container(list).
                     transform([](int v) { return v*3 + 1; })) {
        cout << x << ' ';
    }
    cout << endl;
}
