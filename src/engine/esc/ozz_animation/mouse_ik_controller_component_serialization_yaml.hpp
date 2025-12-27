#pragma once
#include "mouse_ik_controller_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<MouseIKControllerComponentSerialization> {
        static bool decode(const Node &node, MouseIKControllerComponentSerialization &rhs) {
            if (node["enabled"]) {
                rhs.Enabled = node["enabled"].as<bool>();
            }
            if (node["plane_normal"]) {
                const auto normal = node["plane_normal"].as<std::vector<float>>();
                rhs.PlaneNormal = glm::vec3(normal[0], normal[1], normal[2]);
            }
            if (node["plane_distance"]) {
                rhs.PlaneDistance = node["plane_distance"].as<float>();
            }
            if (node["target_offset"]) {
                const auto offset = node["target_offset"].as<std::vector<float>>();
                rhs.TargetOffset = glm::vec3(offset[0], offset[1], offset[2]);
            }
            return true;
        }
    };
}
