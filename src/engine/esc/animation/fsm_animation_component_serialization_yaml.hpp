#pragma once
#include "fsm_animation_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<FsmAnimationComponentSerialization> {
        static bool decode(const Node &node, FsmAnimationComponentSerialization &rhs) {
            if (node["fsm_guid"]) {
                rhs.FsmGuid = node["fsm_guid"].as<std::string>();
            }
            if (node["loop"]) {
                rhs.Loop = node["loop"].as<bool>();
            }
            if (node["animation_speed"]) {
                rhs.AnimationSpeed = node["animation_speed"].as<float>();
            }
            if (node["velocity_based_speed"]) {
                rhs.VelocityBasedSpeed = node["velocity_based_speed"].as<bool>();
            }
            if (node["velocity_speed_scale"]) {
                rhs.VelocitySpeedScale = node["velocity_speed_scale"].as<float>();
            }
            // Directional animation blending
            if (node["directional_walk_forward_guid"]) {
                rhs.DirectionalWalkForwardGuid = node["directional_walk_forward_guid"].as<std::string>();
            }
            if (node["directional_walk_back_guid"]) {
                rhs.DirectionalWalkBackGuid = node["directional_walk_back_guid"].as<std::string>();
            }
            if (node["directional_walk_left_guid"]) {
                rhs.DirectionalWalkLeftGuid = node["directional_walk_left_guid"].as<std::string>();
            }
            if (node["directional_walk_right_guid"]) {
                rhs.DirectionalWalkRightGuid = node["directional_walk_right_guid"].as<std::string>();
            }
            if (node["use_directional_blending"]) {
                rhs.UseDirectionalBlending = node["use_directional_blending"].as<bool>();
            }
            return true;
        }
    };
}
