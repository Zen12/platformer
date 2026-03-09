#define GLM_ENABLE_EXPERIMENTAL
#include "core/src/plugin/plugin_registrar.hpp"
#include "src/register_audio_actions.hpp"
#include "src/register_audio_components.hpp"
#include "src/register_audio_systems.hpp"

struct AudioPlugin {
    static void Register(PluginRegistries& registries) {
        RegisterAudioActions(registries.Actions);
        RegisterAudioComponents(registries.Components);
        RegisterAudioSystems(registries.Systems);
    }
};

REGISTER_PLUGIN(AudioPlugin)
