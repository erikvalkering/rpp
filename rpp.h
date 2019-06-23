#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace rpp {

struct variable {
    static int counter = 0;
    char name = 'a' + counter++;
};

struct dependency : variable {
};

using reaction = std::function<void()>;
using Dependencies = std::unordered_set<const dependency *>;

struct Context {
    std::unordered_map<const dependency *, std::unordered_set<size_t>> reactions{};
    std::vector<std::pair<reaction, Dependencies>> dependencies{};

    Dependencies *current_dependencies = nullptr;
} ctx;

template<typename T>
class observable : public dependency {
public:
    explicit observable(T &&value) : value_{std::forward<T>(value)} {
    }

    const auto &operator()() const {
        if (ctx.current_dependencies) {
            ctx.current_dependencies->insert(this);
        }

        return value_;
    }

    const auto& operator()(nullptr_t) const {
        return value_;
    }

    template<typename U>
    auto &operator=(U &&value) {
        value_ = std::forward<U>(value);

        if (const auto it_reactions = ctx.reactions.find(this); it_reactions != ctx.reactions.end()) {
            const auto reactions = it_reactions->second;
            for (auto& reaction_idx : reactions) {
                const auto &[reaction, _] = ctx.dependencies[reaction_idx];
                reaction();
            }
        }

        return *this;
    }

private:
    T value_;
};

template<class F>
class computed {
public:
    explicit computed(F &&callback) : callback{std::forward<F>(callback)} {
    }
    
    decltype(auto) operator()() const {
        return callback();
    }

private:
    F callback;
};

const auto auto_run = [](auto &&callback) {
    const auto idx = ctx.dependencies.size();

    auto reaction_ = [
        callback = std::forward<decltype(callback)>(callback),
        idx
    ]{
        auto current_dependencies = Dependencies{};
        ctx.current_dependencies = &current_dependencies;

        callback();

        auto &[reaction, dependencies] = ctx.dependencies[idx];

        auto unused_dependencies = std::move(dependencies);
        for (const auto &dependency : *ctx.current_dependencies) {
            unused_dependencies.erase(dependency);
        }
        for (const auto &dependency : unused_dependencies) {
            ctx.reactions[dependency].erase(idx);
        }
        for (const auto &dependency : *ctx.current_dependencies) {
            ctx.reactions[dependency].insert(idx);
        }

        ctx.dependencies[idx].second = move(*ctx.current_dependencies);
        ctx.current_dependencies = nullptr;
    };

    ctx.dependencies.push_back({std::move(reaction_), {}});
    ctx.dependencies[idx].first();
};

} // namespace rpp
