#pragma once
#include <yaml-cpp/yaml.h>
#include "ai_controller_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<AiControllerComponentSerialization>
    {
        static bool decode(const Node &node, AiControllerComponentSerialization &rhs)
        {
            rhs.MaxMovementSpeed = node["max_movement_speed"].as<float>();
            rhs.AccelerationSpeed = node["acceleration_speed"].as<float>();
            rhs.DecelerationSpeed = node["deceleration_speed"].as<float>();
            rhs.JumpHeigh = node["jump_heigh"].as<float>();
            rhs.JumpDuration = node["jump_duration"].as<float>();
            rhs.JumpDownMultiplier = node["jump_down_multiplier"].as<float>();
            rhs.AirControl = node["air_control"].as<float>();
            rhs.Damage = node["damage"].as<float>();
            rhs.AiTargetTransformTag = node["ai_target_transform_tag"].as<std::string>();
            rhs.PathFinderTag = node["path_finder_tag"].as<std::string>();
            rhs.GridTag = node["grid_tag"].as<std::string>();
            return true;
        }
    };
}