#pragma once
#include <yaml-cpp/yaml.h>
#include "ai_controller_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<AiControllerComponentSerialization>
    {
        static bool decode(const Node &node, AiControllerComponentSerialization &rhs)
        {
            rhs.Damage = node["damage"].as<float>();
            rhs.AiTargetTransformTag = node["ai_target_transform_tag"].as<std::string>();
            rhs.PathFinderTag = node["path_finder_tag"].as<std::string>();
            rhs.GridTag = node["grid_tag"].as<std::string>();
            return true;
        }
    };
}