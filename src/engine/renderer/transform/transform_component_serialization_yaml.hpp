#pragma once
#include <yaml-cpp/yaml.h>
#include "transform_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<TransformComponentSerialization>
    {
        static bool decode(const Node &node, TransformComponentSerialization &rhs)
        {
            rhs.position = node["position"].as<glm::vec3>();
            rhs.rotation = node["rotation"].as<glm::vec3>();
            rhs.scale = node["scale"].as<glm::vec3>();
            return true;
        }
    };
}