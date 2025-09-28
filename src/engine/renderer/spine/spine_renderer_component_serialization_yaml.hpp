#pragma once
#include <yaml-cpp/yaml.h>
#include "spine_renderer_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<SpineRenderComponentSerialization>
    {
        static bool decode(const Node &node, SpineRenderComponentSerialization &rhs)
        {
            rhs.SpineGuid = node["spine_data"].as<std::string>();
            return true;
        }
    };
}