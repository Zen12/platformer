#pragma once

#include "directional_light_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<DirectionalLightComponentSerialization>
    {
        static bool decode(const Node &node, DirectionalLightComponentSerialization &rhs)
        {
            if (node["aspect_ratio"])
                rhs.AspectRatio = node["aspect_ratio"].as<float>();
            if (node["near_plane"])
                rhs.NearPlane = node["near_plane"].as<float>();
            if (node["far_plane"])
                rhs.FarPlane = node["far_plane"].as<float>();
            if (node["ortho_size"])
                rhs.OrthoSize = node["ortho_size"].as<float>();
            if (node["follow_tag"])
                rhs.FollowTag = node["follow_tag"].as<std::string>();
            if (node["follow_offset"]) {
                const auto& offset = node["follow_offset"];
                rhs.FollowOffset = glm::vec3(
                    offset[0].as<float>(),
                    offset[1].as<float>(),
                    offset[2].as<float>()
                );
            }
            return true;
        }
    };
}
