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
            if (node["apply_root_motion"]) {
                rhs.ApplyRootMotion = node["apply_root_motion"].as<bool>();
            }
            if (node["root_bone_name"]) {
                rhs.RootBoneName = node["root_bone_name"].as<std::string>();
            }
            return true;
        }
    };
}
