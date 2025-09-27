#pragma once

#include "game_state_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<GameStateData>
    {
        static bool decode(const Node &node, GameStateData &rhs)
        {
            rhs.WinScene = node["win_scene"].as<std::string>();
            rhs.LooseScene = node["lose_scene"].as<std::string>();
            return true;
        }
    };
}