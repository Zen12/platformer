#pragma once

#include "spot_light_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<SpotLightComponentSerialization>
    {
        static bool decode(const Node &node, SpotLightComponentSerialization &rhs)
        {
            if (node["inner_angle"])
                rhs.InnerAngle = node["inner_angle"].as<float>();
            if (node["outer_angle"])
                rhs.OuterAngle = node["outer_angle"].as<float>();
            if (node["range"])
                rhs.Range = node["range"].as<float>();
            if (node["intensity"])
                rhs.Intensity = node["intensity"].as<float>();
            if (node["color"]) {
                const auto& c = node["color"];
                rhs.Color = glm::vec3(
                    c[0].as<float>(),
                    c[1].as<float>(),
                    c[2].as<float>()
                );
            }
            if (node["near_plane"])
                rhs.NearPlane = node["near_plane"].as<float>();
            if (node["far_plane"])
                rhs.FarPlane = node["far_plane"].as<float>();
            return true;
        }
    };
}
