#include "engine/engine.hpp"


int main() {
    std::filesystem::path path = std::filesystem::current_path();
    path.append("assets/");

    Engine engine(path);

    engine.LoadFirstScene();

    while (engine.IsTickable())
    {
        engine.Tick();
    }
}
