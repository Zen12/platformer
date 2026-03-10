#include "core/src/plugin/plugin_registrar.hpp"

struct ScenePlugin {
    static void Register([[maybe_unused]] PluginRegistries& registries) {
    }
};

REGISTER_PLUGIN(ScenePlugin)
