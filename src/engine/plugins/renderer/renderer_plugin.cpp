#define GLM_ENABLE_EXPERIMENTAL
#include "core/src/plugin/plugin_registrar.hpp"
#include "register_core_actions.hpp"
#include "src/action/register_renderer_actions.hpp"
#include "src/register_renderer_systems.hpp"
#include "src/register_renderer_components.hpp"

struct RendererPlugin {
    static void Register(PluginRegistries& registries) {
        RegisterCoreActions(registries.Actions);
        RegisterCoreConditions(registries.Conditions);
        RegisterRendererActions(registries.Actions);
        RegisterRendererComponents(registries.Components);
        RegisterRendererSystems(registries.Systems);
    }
};

REGISTER_PLUGIN(RendererPlugin)
