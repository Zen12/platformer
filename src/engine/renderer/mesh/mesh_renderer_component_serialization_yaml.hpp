#pragma once
#include <yaml-cpp/yaml.h>
#include "mesh_renderer_component_serialization.hpp"

namespace YAML
{
    template <>
    struct convert<MeshRendererComponentSerialization>
    {
        static bool decode(const Node &node, MeshRendererComponentSerialization &rhs)
        {
            rhs.MaterialGuid = node["material"].as<std::string>();
            return true;
        }
    };
}