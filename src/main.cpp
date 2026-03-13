#include "path_utils.hpp"
#include "engine/engine.hpp"
#include "engine/plugins/scene/scene_plugin.hpp"
#include "engine/plugins/renderer/renderer_plugin.hpp"
#include "engine/plugins/animation/animation_plugin.hpp"
#include "engine/plugins/audio/audio_plugin.hpp"
#include "game/game_plugin.hpp"
#include "engine/plugins/ai/ai_plugin.hpp"
#include "engine/plugins/ik/ik_plugin.hpp"
#include "engine/plugins/navigation/navigation_plugin.hpp"
#include "engine/examples/plugin_example/example_plugin.hpp"

std::vector<PluginCallback> GetGamePlugins() {
    return {
        ScenePlugin::Register,
        RendererPlugin::Register,
        AnimationPlugin::Register,
        AudioPlugin::Register,
        GamePlugin::Register,
        AIPlugin::Register,
        IKPlugin::Register,
        NavigationPlugin::Register,
        ExamplePlugin::Register,
    };
}

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

// Global smart pointer for Emscripten loop
static std::unique_ptr<Engine> g_engine;

void RunEngineWebGl() {
    if (!g_engine) {
        g_engine = std::make_unique<Engine>(GetProjectRootPath(), GetGamePlugins());
    }

    if (g_engine->IsTickable()) {
        g_engine->Tick();
        g_engine->WaitForTargetFrameRate();
    } else {
        if (g_engine->IsReloadRequested()) {
            g_engine.reset(); // destroys and sets to nullptr
        } else {
            emscripten_cancel_main_loop();
            g_engine.reset();
        }
    }
}

void LoadEngineWebGL() {
    emscripten_set_main_loop(RunEngineWebGl, 0, true);
}

#endif

bool RunEngineDesktop() {

    Engine engine(GetProjectRootPath(), GetGamePlugins());

    while (engine.IsTickable())
    {
        engine.Tick();
        engine.WaitForTargetFrameRate();
    }

    return engine.IsReloadRequested();
}

void LoadEngineDesktop() {
    const bool reload = RunEngineDesktop();

    if (reload) {
        LoadEngineDesktop();
    }
}

int main() {
#ifdef __EMSCRIPTEN__
    LoadEngineWebGL();
#else
    LoadEngineDesktop();
#endif
}
