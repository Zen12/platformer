#define GLM_ENABLE_EXPERIMENTAL
#include "core/src/plugin/plugin_registrar.hpp"
#include "register_core_actions.hpp"
#include "register_core_systems.hpp"
#include "register_core_components.hpp"

struct CorePlugin {
    static void Register(PluginRegistries& registries) {
        RegisterCoreActions(registries.Actions);
        RegisterCoreConditions(registries.Conditions);
        RegisterCoreSystems(registries.Systems);
        RegisterCoreComponents(registries.Components);
    }
};

REGISTER_PLUGIN(CorePlugin)
