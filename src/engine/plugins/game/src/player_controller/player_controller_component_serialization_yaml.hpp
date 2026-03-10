#pragma once
#include "player_controller_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<PlayerControllerComponentSerialization> {
        static bool decode(const Node& node, PlayerControllerComponentSerialization& rhs) {
            rhs.MoveSpeed = node["move_speed"] ? node["move_speed"].as<float>() : 5.0f;
            rhs.DestinationDistance = node["destination_distance"] ? node["destination_distance"].as<float>() : 1.0f;
            return true;
        }
    };
}
