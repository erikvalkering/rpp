#include <functional>
#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace rpp {

auto counter = 0;

struct variable {
    char name = 'a' + counter++;
};

using observer = std::function<void ()>;

struct dependency : variable {
    std::vector<std::shared_ptr<observer>> observers;
};

struct Context {
    std::stack<std::shared_ptr<observer>> current_observers{};
};

auto ctx = Context{};

template<typename T>
class observable : public dependency {
public:
    explicit observable(T &&value) : value_{std::forward<T>(value)} {
    }

    const auto &operator()() const {
        if (!empty(ctx.current_observers)) {
            observers.push_back(ctx.current_observers.top());
        }

        return value_;
    }

    const auto &operator()(nullptr_t) const {
        return value_;
    }

    template<typename U>
    auto &operator=(U &&value) {
        value_ = std::forward<U>(value);

        // 1. recursively call all the observers
        // 1a. each observer clears its cache (if its a computed observer)
        // 1b. each observer appends its reaction to a list of reactions (as a tracked_observer)
        // 1c. each observer recurses its child observers
        // 1d. each observer clears its list of observers
        auto reactions = std::vector<std::shared_ptr<observer>>{};
        for (const auto &observer : observers) {
            (*observer)(reactions);
        }

        observers.clear();

        // 2. call each reaction in the list of reactions populated in step 1
        for (const auto &reaction : reactions) {
            (*reaction)();
        }

        return *this;
    }

private:
    T value_;
};

//template<class F>
//class computed {
//public:
//    explicit computed(F &&callback) : callback{std::forward<F>(callback)} {
//    }
//    
//    decltype(auto) operator()() const {
//        if (!value) {
//            value = callback();
//
//            // TODO: add a recalculate reac
//        }
//
//        if (!empty(ctx.current_dependencies)) {
//            ctx.current_dependencies.top().insert(this);
//        }
//
//        return *value;
//    }
//
//private:
//    F callback;
//
//    using value_type = decltype(callback());
//    std::optional<value_type> value;
//};

template<class Reaction>
struct tracking_observer : std::enable_shared_from_this<tracking_observer> {
    Reaction reaction;

    decltype(auto) operator()() const {
        ctx.current_observers.push(shared_from_this());
        reaction();
        ctx.current_observers.pop();
    }

    auto execute_and_observe() const && {
        const auto &observer = *std::make_shared<tracking_observer>(std::move(*this));
        observer();
    }
};

const auto autorun = [](auto &&reaction) {
    //using observer_t = decltype(observer);
    //const auto tracking_observer = std::make_shared<tracking_observer<observer_t &&>>(
    //    std::forward<decltype(observer)>(observer)
    //);

    //(*tracking_observer)();
    tracking_observer{std::forward<decltype(reaction)>(reaction)}.execute_and_observe();
};

} // namespace rpp
