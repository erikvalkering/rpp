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

    auto_run([&] { cout << "auto_run 1: z == " << z() << endl; });
    auto_run([&] { cout << "auto_run 2: z == " << z() << endl; });

    cout << "x = 10" << endl;
             x = 10;
    cout << "y = 12" << endl;
             y = 12;

    cout << "b = false" << endl;
             b = false;

    cout << "x = 100" << endl;
             x = 100;
    cout << "y = 102" << endl;
             y = 102;

    cout << "b = true" << endl;
             b = true;
}

auto test() {
    using namespace std;
    using namespace rpp;

    //
    // [a] <--- (b) <-
    //       `- (c) <-`- (d) <-
    //         `---------------`{autorun}

    auto a = observable{1};
    auto b = computed{[&] { return a(); }};
    auto c = computed{[&] { return a(); }};
    auto d = computed{[&] { return b() + c(); }};
    
    autorun([&] { cout << a() + d() << endl; });
}

auto test_lazy() {
    using namespace std;
    using namespace rpp;

    auto a = observable{1};

    auto b = computed{[&] { cout << a() << endl; }};

    cout << b() << endl; // cout << 1 << endl;

    a = 2; // nothing

    cout << b() << endl; // cout << 2 << endl;
}

auto test_unregister() {
    using namespace std;
    using namespace rpp;

    auto a = observable{1};

    auto unregister = autorun([&] { cout << a() << endl; });
    a = 2; // cout << 2 << endl;

    unregister();
    a = 3; // nothing
}

auto test_scope() {
    using namespace std;
    using namespace rpp;

    auto a = observable{1};

    {
        auto b = computed{[&] { return a(); }};

        // b goes out of scope, but is not observing a
        // therefore, no unregistering needs to be done
    }

    a = 2; // nothing
}

auto test_auto_unregister() {
    using namespace std;
    using namespace rpp;

    auto a = observable{1};

    {
        auto b = computed{[&] { return a(); }};
        autorun([&] { cout << b() << endl; });

        a = 2; // cout << 2 << endl;

        // b goes out of scope, unregisters itself
        // autorun is dangling and should therefore be unregistered as well
    }

    a = 3; // nothing
}

auto test_auto_unregister_dangling() {
    using namespace std;
    using namespace rpp;

    auto a = observable{1};

    {
        auto b = computed{[&] { return a(); }};
        autorun([&] { cout << a() << b() << endl; });

        a = 2; // cout << 2 << 2 << endl;

        // b goes out of scope, unregisters itself
        // because invoking the autorun would be undefined behavior, should we also unregister it?
    }

    a = 3; // nothing
}
