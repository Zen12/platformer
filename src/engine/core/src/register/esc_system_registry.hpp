#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "esc/esc_core.hpp"

struct EscSystemContext;

class EscSystemRegistry final {
public:
    using BuilderFn = std::function<std::unique_ptr<ISystem>(const EscSystemContext&)>;

    void Register(const std::string& typeName, BuilderFn builder, int priority) {
        _builders[typeName] = std::move(builder);
        _priorities[typeName] = priority;
    }

    [[nodiscard]] std::unique_ptr<ISystem> Build(const std::string& typeName, const EscSystemContext& ctx) const {
        auto it = _builders.find(typeName);
        if (it == _builders.end()) return nullptr;
        return it->second(ctx);
    }

    [[nodiscard]] std::vector<std::unique_ptr<ISystem>> BuildAll(const EscSystemContext& ctx) const {
        std::map<int, std::vector<const std::pair<const std::string, BuilderFn>*>> ordered;
        for (const auto& entry : _builders) {
            int prio = 0;
            auto pit = _priorities.find(entry.first);
            if (pit != _priorities.end()) {
                prio = pit->second;
            }
            ordered[prio].push_back(&entry);
        }

        std::vector<std::unique_ptr<ISystem>> systems;
        for (const auto& [priority, entries] : ordered) {
            for (const auto* entry : entries) {
                auto system = entry->second(ctx);
                if (system) {
                    systems.push_back(std::move(system));
                }
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
};
