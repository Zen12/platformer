#pragma once

#include "../../asset/serialization/serialization_component.hpp"

struct GameStateData final : public ComponentSerialization {
    std::string WinScene;
    std::string LooseScene;
};