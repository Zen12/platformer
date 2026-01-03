#pragma once
#include <yaml-cpp/yaml.h>
#include "navmesh_agent_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<NavmeshAgentComponentSerialization> {
        static bool decode(const Node &node, NavmeshAgentComponentSerialization &rhs) {
            if (node["radius"]) {
                rhs.Radius = node["radius"].as<float>();
            }
            if (node["max_speed"]) {
                rhs.MaxSpeed = node["max_speed"].as<float>();
            }
            if (node["rotation_speed"]) {
                rhs.RotationSpeed = node["rotation_speed"].as<float>();
            }
            if (node["enabled"]) {
                rhs.Enabled = node["enabled"].as<bool>();
            }
            return true;
        }
    };
}
