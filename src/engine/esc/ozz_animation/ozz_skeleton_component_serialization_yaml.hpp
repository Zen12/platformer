#pragma once
#include "ozz_skeleton_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<OzzSkeletonComponentSerialization> {
        static bool decode(const Node &node, OzzSkeletonComponentSerialization &rhs) {
            if (!node.IsMap()) {
                return false;
            }

            if (node["skeleton_guid"]) {
                rhs.SkeletonGuid = node["skeleton_guid"].as<std::string>();
            }

            return true;
        }
    };
}
