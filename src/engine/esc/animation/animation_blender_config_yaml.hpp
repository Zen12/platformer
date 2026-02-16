#pragma once
#include <yaml-cpp/yaml.h>
#include "animation_blender_config.hpp"
#include "guid_yaml.hpp"

namespace YAML {
    template<>
    struct convert<AnimationBlendMode> {
        static bool decode(const Node& node, AnimationBlendMode& rhs) {
            const auto str = node.as<std::string>("simple");
            if (str == "transition") {
                rhs = AnimationBlendMode::Transition;
            } else if (str == "directional") {
                rhs = AnimationBlendMode::Directional;
            } else {
                rhs = AnimationBlendMode::Simple;
            }
            return true;
        }
    };

    template<>
    struct convert<AnimationBlenderConfig> {
        static bool decode(const Node& node, AnimationBlenderConfig& rhs) {
            if (node["mode"]) {
                rhs.Mode = node["mode"].as<AnimationBlendMode>();
            }
            if (node["transition_duration"]) {
                rhs.TransitionDuration = node["transition_duration"].as<float>();
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
            if (node["directional_forward_guid"]) {
                rhs.DirectionalForwardGuid = node["directional_forward_guid"].as<Guid>();
            }
            if (node["directional_back_guid"]) {
                rhs.DirectionalBackGuid = node["directional_back_guid"].as<Guid>();
            }
            if (node["directional_left_guid"]) {
                rhs.DirectionalLeftGuid = node["directional_left_guid"].as<Guid>();
            }
            if (node["directional_right_guid"]) {
                rhs.DirectionalRightGuid = node["directional_right_guid"].as<Guid>();
            }
            return true;
        }
    };
}
