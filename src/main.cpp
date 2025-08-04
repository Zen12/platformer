#include "engine/engine.hpp"

int main()
{
    std::string projectRoot = ASSETS_PATH;

    Engine engine(projectRoot);

    engine.LoadFirstScene();

    while (engine.IsTickable())
    {
        engine.Tick();
    }
}
