#include "core/src/plugin/plugin_registrar.hpp"
#include "register_core_actions.hpp"
#include "esc/register_core_systems.hpp"

struct CorePlugin {
    static void Register(PluginRegistries& registries) {
        RegisterCoreActions(registries.Actions);
        RegisterCoreConditions(registries.Conditions);
        RegisterCoreSystems(registries.Systems);
    }
};

REGISTER_PLUGIN(CorePlugin)
