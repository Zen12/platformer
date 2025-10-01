#pragma once
#include <yaml-cpp/yaml.h>
#include "character_movement_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<CharacterMovementComponentSerialization>
    {
        static bool decode(const Node &node, CharacterMovementComponentSerialization &rhs)
        {
            rhs.MaxMovementSpeed = node["max_movement_speed"].as<float>();
            rhs.AccelerationSpeed = node["acceleration_speed"].as<float>();
            rhs.DecelerationSpeed = node["deceleration_speed"].as<float>();
            rhs.JumpHeight = node["jump_heigh"].as<float>();
            rhs.JumpDuration = node["jump_duration"].as<float>();
            rhs.JumpDownMultiplier = node["jump_down_multiplier"].as<float>();
            rhs.AirControl = node["air_control"].as<float>();
            return true;
        }
    };
}