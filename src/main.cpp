#include "engine/engine.hpp"


void RunEngine() {
    std::filesystem::path path = std::filesystem::current_path();
    path.append("assets/");

    Engine engine(path);

    engine.LoadFirstScene();

    while (engine.IsTickable())
    {
        engine.Tick();
    }

    if (engine.IsReloadRequested()) {
        RunEngine();
    }
}

int main() {

    RunEngine();
}
