#include "core/src/plugin/plugin_registrar.hpp"
#include "register_audio_actions.hpp"
#include "register_audio_components.hpp"

struct AudioPlugin {
    static void Register(PluginRegistries& registries) {
        RegisterAudioActions(registries.Actions);
        RegisterAudioComponents(registries.Components);
    }
};

REGISTER_PLUGIN(AudioPlugin)
