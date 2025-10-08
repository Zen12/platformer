#pragma once
#include <yaml-cpp/yaml.h>
#include "shoot_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<ShootComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] ShootComponentSerialization &rhs)
        {
            if (node["bone_name"]) {
                rhs.BoneName = node["bone_name"].as<std::string>();
            }
            if (node["raycast_distance"]) {
                rhs.RaycastDistance = node["raycast_distance"].as<float>();
            }
            return true;
        }
    };
}
