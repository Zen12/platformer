#include "path_utils.hpp"
#include "engine/engine.hpp"


#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

// Global smart pointer for Emscripten loop
static std::unique_ptr<Engine> g_engine;

void RunEngineWebGl() {
    if (!g_engine) {
        g_engine = std::make_unique<Engine>(GetProjectRootPath());
        g_engine->LoadFirstScene();
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
#endif

bool RunEngineDesktop() {

    Engine engine(GetProjectRootPath());

    engine.LoadFirstScene();

    while (engine.IsTickable())
    {
        engine.Tick();
        engine.WaitForTargetFrameRate();
    }

    return engine.IsReloadRequested();
}

void LoadEngine() {
#ifndef __EMSCRIPTEN__
    const bool reload = RunEngineDesktop();

    if (reload) {
        LoadEngine();
    }
#else
    emscripten_set_main_loop(RunEngineWebGl, 0, true);
#endif

}

int main() {
    LoadEngine();
}
