#include "sona/log.hpp"
#include "sona/backtrace.hpp"
#include "sona/linq.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

int main() {
    using namespace std;
    using namespace sona;

    vector<string> vec { "diao******", "du***", "chentianze", "lu***zh***",
                         "liu****", "liu****"};

    for (int x : linq::from_container(vec).
                     transform([](string const& str) { return str.size(); }).
                     transform([](int v) { return v*3 + 1; })) {
        cout << x << ' ';
    }

    cout << endl;

    for (auto const& x : linq::numeric_range<int>(1, 101).
                             transform([](int v) { return to_string(v); } ).
                             transform([](string const& s) { return s + s; })) {
        cout << x << ' ';
    }
}
