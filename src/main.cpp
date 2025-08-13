#include "path_utils.hpp"
#include "engine/engine.hpp"



void RunEngine() {

    Engine engine(GetProjectRootPath());

    engine.LoadFirstScene();

    while (engine.IsTickable())
    {
        engine.Tick();
        engine.WaitForTargetFrameRate();
    }

    if (engine.IsReloadRequested()) {
        RunEngine();
    }
}

int main() {

    RunEngine();
}
