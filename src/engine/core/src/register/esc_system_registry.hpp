#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "esc/esc_core.hpp"
#include "esc/system_phase.hpp"

struct EscSystemContext;

class EscSystemRegistry final {
public:
    using BuilderFn = std::function<std::unique_ptr<ISystem>(const EscSystemContext&)>;

    void Register(const std::string& typeName, BuilderFn builder, int priority, SystemPhase phase = SystemPhase::RENDER) {
        _builders[typeName] = std::move(builder);
        _priorities[typeName] = priority;
        _phases[typeName] = phase;
    }

    template<typename TSystem, typename F>
    void Register(const std::string& typeName, F&& factory, int priority, SystemPhase phase = SystemPhase::RENDER) {
        static_assert(std::is_base_of_v<ISystem, TSystem>);
        Register(typeName,
            [f = std::forward<F>(factory)](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
                return f(ctx);
            },
            priority,
            phase
        );
    }

    [[nodiscard]] std::unique_ptr<ISystem> Build(const std::string& typeName, const EscSystemContext& ctx) const {
        auto it = _builders.find(typeName);
        if (it == _builders.end()) return nullptr;
        return it->second(ctx);
    }

    [[nodiscard]] std::vector<std::unique_ptr<ISystem>> BuildAll(const EscSystemContext& ctx) const {
        std::map<int, std::vector<std::string>> ordered;
        for (const auto& entry : _builders) {
            int prio = 0;
            auto pit = _priorities.find(entry.first);
            if (pit != _priorities.end()) prio = pit->second;
            ordered[prio].push_back(entry.first);
        }

        std::vector<std::unique_ptr<ISystem>> systems;
        for (const auto& [priority, names] : ordered) {
            for (const auto& name : names) {
                auto system = _builders.at(name)(ctx);
                if (system) systems.push_back(std::move(system));
            }
        }
        return systems;
    }

    [[nodiscard]] std::vector<std::unique_ptr<ISystem>> BuildByPhase(const EscSystemContext& ctx, SystemPhase phase) const {
        std::map<int, std::vector<std::string>> ordered;
        for (const auto& entry : _builders) {
            auto phaseIt = _phases.find(entry.first);
            SystemPhase entryPhase = SystemPhase::RENDER;
            if (phaseIt != _phases.end()) entryPhase = phaseIt->second;
            if (entryPhase != phase) continue;

            int prio = 0;
            auto pit = _priorities.find(entry.first);
            if (pit != _priorities.end()) prio = pit->second;
            ordered[prio].push_back(entry.first);
        }

        std::vector<std::unique_ptr<ISystem>> systems;
        for (const auto& [priority, names] : ordered) {
            for (const auto& name : names) {
                auto system = _builders.at(name)(ctx);
                if (system) systems.push_back(std::move(system));
            }
        }
        return systems;
    }

    [[nodiscard]] bool HasType(const std::string& typeName) const {
        return _builders.find(typeName) != _builders.end();
    }

private:
    std::unordered_map<std::string, BuilderFn> _builders;
    std::unordered_map<std::string, int> _priorities;
    std::unordered_map<std::string, SystemPhase> _phases;
};
