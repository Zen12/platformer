#pragma once
#include <yaml-cpp/yaml.h>
#include "light_2d_component_serialization.hpp"

namespace YAML
{
    template <>
    struct convert<Light2dComponentSerialization>
    {
        static bool decode(const Node &node, Light2dComponentSerialization &rhs) {

            rhs.CenterTransform = node["center"].as<std::string>();
            rhs.Offset = node["offset"].as<glm::vec3>();
            rhs.Color = node["color"].as<glm::vec3>();
            rhs.Radius = node["radius"].as<float>();
            rhs.Segments = node["segments"].as<int>();
            rhs.MaxAngle = node["max_angle"].as<float>();
            rhs.StartAngle = node["start_angle"].as<float>();

            return true;
        }
    };
}