#pragma once
#include <yaml-cpp/yaml.h>
#include "character_controller_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<CharacterControllerComponentSerialization>
    {
        static bool decode(const Node &node, CharacterControllerComponentSerialization &rhs)
        {
            rhs.MaxMovementSpeed = node["max_movement_speed"].as<float>();
            rhs.AccelerationSpeed = node["acceleration_speed"].as<float>();
            rhs.DecelerationSpeed = node["deceleration_speed"].as<float>();
            rhs.JumpHeigh = node["jump_heigh"].as<float>();
            rhs.JumpDuration = node["jump_duration"].as<float>();
            rhs.JumpDownMultiplier = node["jump_down_multiplier"].as<float>();
            rhs.AirControl = node["air_control"].as<float>();
            rhs.Damage = node["damage"].as<float>();
            return true;
        }
    };
}