#include "path_utils.hpp"
#include "engine/engine.hpp"


#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

// Global pointer for Emscripten loop
static Engine* g_engine = nullptr;

void RunEngineWebGl() {
    if (g_engine == nullptr) {
        g_engine = new Engine(GetProjectRootPath());
        g_engine->LoadFirstScene();
    }

    if (g_engine->IsTickable()) {
        g_engine->Tick();
        g_engine->WaitForTargetFrameRate();
    } else {
        if (g_engine->IsReloadRequested()) {
            delete g_engine;
            g_engine = nullptr;
        } else {
            emscripten_cancel_main_loop();
            delete g_engine;
            g_engine = nullptr;
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
