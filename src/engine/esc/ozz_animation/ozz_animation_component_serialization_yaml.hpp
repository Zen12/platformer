#pragma once
#include "ozz_animation_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<OzzAnimationComponentSerialization> {
        static bool decode(const Node &node, OzzAnimationComponentSerialization &rhs) {
            if (!node.IsMap()) {
                return false;
            }

            if (node["animation_guid"]) {
                rhs.AnimationGuid = node["animation_guid"].as<std::string>();
            }
            if (node["playback_speed"]) {
                rhs.PlaybackSpeed = node["playback_speed"].as<float>();
            }
            if (node["loop"]) {
                rhs.Loop = node["loop"].as<bool>();
            }
            if (node["playing"]) {
                rhs.Playing = node["playing"].as<bool>();
            }

            return true;
        }
    };
}
