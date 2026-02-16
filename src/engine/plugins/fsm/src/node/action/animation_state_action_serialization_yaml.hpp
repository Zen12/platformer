#pragma once
#include "animation_state_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<AnimationStateActionSerialization> {
        static bool decode(const Node &node, AnimationStateActionSerialization &rhs) {
            rhs.AnimationGuid = node["animation_guid"].as<std::string>();
            if (node["on_complete_trigger"]) {
                rhs.OnCompleteTrigger = node["on_complete_trigger"].as<std::string>();
            }
            if (node["loop"]) {
                rhs.Loop = node["loop"].as<bool>();
            }
            if (node["animation_speed"]) {
                rhs.AnimationSpeed = node["animation_speed"].as<float>();
            }
            if (node["disable_velocity_speed"]) {
                rhs.DisableVelocitySpeed = node["disable_velocity_speed"].as<bool>();
            }
            if (node["disable_movement"]) {
                rhs.DisableMovement = node["disable_movement"].as<bool>();
            }
            if (node["disable_movement_duration"]) {
                rhs.DisableMovementDuration = node["disable_movement_duration"].as<float>();
            }
            if (node["use_directional_blending"]) {
                rhs.UseDirectionalBlending = node["use_directional_blending"].as<bool>();
            }
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
            return true;
        }
    };
}
