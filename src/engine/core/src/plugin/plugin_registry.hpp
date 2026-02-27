#pragma once
#include <functional>
#include <vector>

#include "../register/fsm_action_registry.hpp"
#include "../register/fsm_condition_registry.hpp"
#include "../register/esc_system_registry.hpp"
#include "entity/component_registry.hpp"

struct PluginRegistries {
    FsmActionRegistry& Actions;
    FsmConditionRegistry& Conditions;
    ComponentRegistry& Components;
    EscSystemRegistry& Systems;
};

class PluginRegistry final {
public:
    static PluginRegistry& Instance() {
        static PluginRegistry instance;
        return instance;
    }

    void AddCallback(std::function<void(PluginRegistries&)> callback) {
        _callbacks.push_back(std::move(callback));
    }

    void RegisterAll(PluginRegistries& registries) {
        for (const auto& callback : _callbacks) {
            callback(registries);
        }
    }

private:
    PluginRegistry() = default;
    std::vector<std::function<void(PluginRegistries&)>> _callbacks;
};
