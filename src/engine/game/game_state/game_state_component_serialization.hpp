#pragma once

#include "../../entity/component_serialization.hpp"

struct GameStateData final : public ComponentSerialization {
    std::string WinScene;
    std::string LooseScene;
};