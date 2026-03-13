#pragma once
#include "plugin/plugin_registry.hpp"
#include "register_core_actions.hpp"

struct CorePlugin {
    static void Register(PluginRegistries& registries) {
        RegisterCoreActions(registries.Actions);
        RegisterCoreConditions(registries.Conditions);
    }
};
