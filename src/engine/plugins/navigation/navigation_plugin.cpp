#define GLM_ENABLE_EXPERIMENTAL
#include "core/src/plugin/plugin_registrar.hpp"
#include "src/register_navigation_systems.hpp"
#include "src/register_navigation_components.hpp"

struct NavigationPlugin {
    static void Register(PluginRegistries& registries) {
        RegisterNavigationComponents(registries.Components);
        RegisterNavigationSystems(registries.Systems);
    }
};

REGISTER_PLUGIN(NavigationPlugin)
