#define GLM_ENABLE_EXPERIMENTAL
#include "core/src/plugin/plugin_registrar.hpp"
#include "src/action/register_game_actions.hpp"
#include "src/register_game_systems.hpp"
#include "src/register_game_components.hpp"

struct GamePlugin {
    static void Register(PluginRegistries& registries) {
        RegisterGameActions(registries.Actions);
        RegisterGameSystems(registries.Systems);
        RegisterGameComponents(registries.Components);
    }
};

REGISTER_PLUGIN(GamePlugin)
