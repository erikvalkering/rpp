#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <unordered_set>
#include <vector>

namespace rpp {

// TODO: consistency in casing
struct Dependency {};

// TODO: rename to Observer?
using Reaction = std::function<void ()>;
using Dependencies = std::unordered_set<const Dependency *>;

struct Context {
    std::map<const Dependency *, std::unordered_set<const Reaction *>> reactions{};
    std::vector<std::pair<Reaction, Dependencies>> dependencies{};

    Dependencies *current_dependencies = nullptr;
} ctx;

template<typename T>
class observable : public Dependency {
public:
    explicit observable(T value) : value_{value} {}

	// TODO: all the overloads
    const auto &operator()() const {
        if (ctx.current_dependencies) {
            ctx.current_dependencies->insert(this); // TODO: or .emplace()?
        }
        
        return value_;
    }

    const auto &operator()(nullptr_t) const {
        return value_;
    }

    template<typename U>
    auto &operator=(U &&value) {
        value_ = std::forward<U>(value);

        if (auto reactions = ctx.reactions.find(this); reactions != ctx.reactions.end()) {
            for (auto &reaction : reactions->second) {
                (*reaction)();
            }
        }

        return *this;
    }

private:
    T value_;
};

const auto auto_run = [](auto &&callback) {
	const auto idx = ctx.dependencies.size();

	auto reaction_ = [
		callback = std::forward<decltype(callback)>(callback),
		idx
	] {
		auto current_dependencies = Dependencies{};
		ctx.current_dependencies = &current_dependencies;

        callback();

		auto &[reaction, dependencies] = ctx.dependencies[idx];

        auto unused_dependencies = std::move(dependencies);
		for (auto& dependency : *ctx.current_dependencies) {
			unused_dependencies.erase(dependency);
		}
		for (auto &dependency : unused_dependencies) {
            ctx.reactions[dependency].erase(&reaction);
        }
        for (auto &dependency : *ctx.current_dependencies) {
            ctx.reactions[dependency].insert(&reaction);
        }

        ctx.dependencies[idx].second = move(*ctx.current_dependencies);
        ctx.current_dependencies = nullptr;
    };

	ctx.dependencies.push_back({std::move(reaction_), {}});
	ctx.dependencies[idx].first();
};

} // namespace rpp

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
