#pragma once
#include <yaml-cpp/yaml.h>
#include "simple_animation_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<SimpleAnimationComponentSerialization> {
        static bool decode(const Node &node, SimpleAnimationComponentSerialization &rhs) {
            if (node["animation"]) {
                rhs.AnimationGuid = node["animation"].as<std::string>();
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
