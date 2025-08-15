#include "path_utils.hpp"
#include "engine/engine.hpp"


bool RunEngine() {

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
    const bool reload = RunEngine();

    if (reload) {
        LoadEngine();
    }
}

int main() {
    LoadEngine();
}
