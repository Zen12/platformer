#pragma once
#include <yaml-cpp/yaml.h>
#include "rect_transform_follower_component_serialization.hpp"

namespace YAML
{
    template <>
    struct convert<RectTransformFollowerSerialization>
    {
        static bool decode(const Node &node, RectTransformFollowerSerialization &rhs)
        {
            rhs.UseCreator = node["use_creator"].as<bool>();
            rhs.Offset = node["offset"].as<glm::vec2>();
            if (node["target"])
                rhs.Target = node["target"].as<std::string>();
            return true;
        }
    };
}