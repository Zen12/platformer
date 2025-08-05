#include "engine/engine.hpp"


void RunEngine() {
    std::filesystem::path path = std::filesystem::current_path();
    path.append("assets/");
    std::string projectRoot = path.string();

#ifndef NDEBUG
#ifdef TEST_ASSET_PATH
    projectRoot = TEST_ASSET_PATH;
#endif
#endif

    std::cout << "Load project from: " << projectRoot << "\n";
    Engine engine(projectRoot);

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
