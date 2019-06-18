#include <functional>
#include <map>
#include <unordered_set>
#include <vector>

namespace rpp {

    // TODO: consistency in casing
    struct Dependency {};

    // TODO: rename to Observer?
    using Reaction = std::function<void()>;
    using Dependencies = std::unordered_set<const Dependency*>;

    struct Context {
        std::map<const Dependency*, std::unordered_set<const Reaction*>> reactions{};
        std::vector<std::pair<Reaction, Dependencies>> dependencies{};

        Dependencies* current_dependencies = nullptr;
    } ctx;

    template<typename T>
    class observable : public Dependency {
    public:
        explicit observable(T value) : value_{ value } {}

        // TODO: all the overloads
        const auto& operator()() const {
            if (ctx.current_dependencies) {
                ctx.current_dependencies->insert(this); // TODO: or .emplace()?
            }

            return value_;
        }

        const auto& operator()(nullptr_t) const {
            return value_;
        }

        template<typename U>
        auto& operator=(U&& value) {
            value_ = std::forward<U>(value);

            if (auto reactions = ctx.reactions.find(this); reactions != ctx.reactions.end()) {
                for (auto& reaction : reactions->second) {
                    (*reaction)();
                }
            }

            return *this;
        }

    private:
        T value_;
    };

    const auto auto_run = [](auto&& callback) {
        const auto idx = ctx.dependencies.size();

        auto reaction_ = [
            callback = std::forward<decltype(callback)>(callback),
                idx
        ]{
            auto current_dependencies = Dependencies{};
            ctx.current_dependencies = &current_dependencies;

            callback();

            auto& [reaction, dependencies] = ctx.dependencies[idx];

            auto unused_dependencies = std::move(dependencies);
            for (auto& dependency : *ctx.current_dependencies) {
                unused_dependencies.erase(dependency);
            }
            for (auto& dependency : unused_dependencies) {
                ctx.reactions[dependency].erase(&reaction);
            }
            for (auto& dependency : *ctx.current_dependencies) {
                ctx.reactions[dependency].insert(&reaction);
            }

            ctx.dependencies[idx].second = move(*ctx.current_dependencies);
            ctx.current_dependencies = nullptr;
        };

        ctx.dependencies.push_back({ std::move(reaction_), {} });
        ctx.dependencies[idx].first();
    };

} // namespace rpp
