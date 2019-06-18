#include "rpp.h"

#include <iostream>

int main() {
    using namespace std;
    using namespace rpp;

    auto b = observable{true};
    auto x = observable{0};
    auto y = observable{2};
    
    // auto z = computed{[&](auto &ctx){
    //     if (b(ctx))
    //         return x(ctx) + 1;
    //     else
    //         return y(ctx) + 1;
    // }};
    
    // b = true; x = 0; y = 2; z = 1
//     auto_run([&](auto &ctx) {
//         cout << "b = " << b(ctx) << "; ";
//         cout << "x = " << x(nullptr) << "; ";
//         cout << "y = " << y(nullptr) << "; ";
// //        cout << "z = " << z(ctx) << endl;
//     });
//     auto_run([&] {
//         cout << "b = " << b() << "; ";
//         cout << "x = " << x(nullptr) << "; ";
//         cout << "y = " << y(nullptr) << "; ";
// //        cout << "z = " << z() << endl;
//     });

    // "x = 0"
    auto_run([&] {
        if (b())
            cout << "x = " << x() << endl;
        else
            cout << "y = " << y() << endl;
    });

    cout << "x = 10: --> ";
    x = 10; // "x = 10"
    cout << endl;
    cout << "y = 12: --> ";
    y = 12;
    cout << endl;

    cout << "b = false: --> ";
    b = false; // "y = 12"
    cout << endl;

    cout << "x = 100: --> ";
    x = 100;
    cout << endl;
    cout << "y = 102: --> ";
    y = 102; // "y = 102"
    cout << endl;

    cout << "b = true: --> ";
    b = true; // "x = 100"
    cout << endl;

    // x = 10; // b = true; x = 10; y = 2; z = 11
    // y = 12;

    // b = false; // b = false; x = 10; y = 12; z = 13
    // x = 100;
    // y = 102; // b = false; x = 100; y = 102; z = 103

    // b = true; // b = true; x = 100; y = 102; z = 101
}
