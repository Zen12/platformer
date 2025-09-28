#pragma once
#include <yaml-cpp/yaml.h>
#include "box_collider2d_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<Box2dColliderSerialization>
    {
        static bool decode(const Node &node, Box2dColliderSerialization &rhs) {
            rhs.scale = node["size"].as<glm::vec3>();
            if (node["translate"].IsDefined())
                rhs.translate = node["translate"].as<glm::vec3>();
            return true;
        }
    };
}