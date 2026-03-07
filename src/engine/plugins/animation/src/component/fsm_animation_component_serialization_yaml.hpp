#pragma once
#include "fsm_animation_component_serialization.hpp"
#include "animation_blender_config_yaml.hpp"
#include <yaml-cpp/yaml.h>
#include "guid_yaml.hpp"

namespace YAML {
    template <>
    struct convert<FsmAnimationComponentSerialization> {
        static bool decode(const Node &node, FsmAnimationComponentSerialization &rhs) {
            if (node["fsm_guid"]) {
                rhs.FsmGuid = node["fsm_guid"].as<Guid>();
            }
            if (node["loop"]) {
                rhs.Loop = node["loop"].as<bool>();
            }
            if (node["blender"]) {
                rhs.BlenderConfig = node["blender"].as<AnimationBlenderConfig>();
            }
            return true;
        }
    };
}
