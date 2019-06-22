#include "rpp.h"

#include <iostream>

int main() {
    using namespace std;
    using namespace rpp;

    auto b = observable{true};
    auto x = observable{0};
    auto y = observable{2};
    
    auto z = computed{[&] {
        cout << "[computing z...] ";

        if (b())
            return x();
        else
            return y();
    }};

    cout << "auto_run 1";
    auto_run([&] { cout << "z = " << z(); });
    cout << endl;
    cout << "auto_run 2";
    auto_run([&] { cout << "z + 1 = " << z() << " "; });
    cout << endl;

    cout << "x = 10: -->";
    x = 10; // "z = 10"
    cout << endl;
    cout << "y = 12: -->";
    y = 12;
    cout << endl;

    cout << "b = false: -->";
    b = false; // "z = 12"
    cout << endl;

    cout << "x = 100: -->";
    x = 100;
    cout << endl;
    cout << "y = 102: -->";
    y = 102; // "z = 102"
    cout << endl;

    cout << "b = true: -->";
    b = true; // "z = 100"
    cout << endl;
}
