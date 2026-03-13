#pragma once

#include "core/core_plugin.hpp"
#include "plugins/scene/scene_plugin.hpp"
#include "plugins/ai/ai_plugin.hpp"
#include "plugins/renderer/renderer_plugin.hpp"
#include "plugins/animation/animation_plugin.hpp"
#include "plugins/audio/audio_plugin.hpp"
#include "plugins/game/game_plugin.hpp"
#include "plugins/ik/ik_plugin.hpp"
#include "plugins/navigation/navigation_plugin.hpp"
#include "examples/plugin_example/example_plugin.hpp"

inline void RegisterAllPlugins(PluginRegistries& registries) {
    CorePlugin::Register(registries);
    ScenePlugin::Register(registries);
    RendererPlugin::Register(registries);
    AnimationPlugin::Register(registries);
    AudioPlugin::Register(registries);
    GamePlugin::Register(registries);
    AIPlugin::Register(registries);
    IKPlugin::Register(registries);
    NavigationPlugin::Register(registries);
    ExamplePlugin::Register(registries);
}
