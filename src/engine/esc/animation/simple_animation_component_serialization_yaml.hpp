#pragma once
#include <yaml-cpp/yaml.h>
#include "simple_animation_component_serialization.hpp"
#include "../../system/guid_yaml.hpp"

namespace YAML {
    template <>
    struct convert<SimpleAnimationComponentSerialization> {
        static bool decode(const Node &node, SimpleAnimationComponentSerialization &rhs) {
            if (node["animation"]) {
                rhs.AnimationGuid = node["animation"].as<Guid>();
            }
            return true;
        }
    };
}
