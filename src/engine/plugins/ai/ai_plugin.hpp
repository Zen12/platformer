#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "core/src/plugin/plugin_registry.hpp"
#include "src/register_ai_components.hpp"
#include "src/register_ai_systems.hpp"

struct AIPlugin {
    static void Register(PluginRegistries& registries) {
        RegisterAIComponents(registries.Components);
        RegisterAISystems(registries.Systems);
    }
};
