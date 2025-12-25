#pragma once
#include <yaml-cpp/yaml.h>
#include "skinned_mesh_renderer_component_serialization.hpp"

namespace YAML
{
    template <>
    struct convert<SkinnedMeshRendererComponentSerialization>
    {
        static bool decode(const Node &node, SkinnedMeshRendererComponentSerialization &rhs)
        {
            rhs.MaterialGuid = node["material"].as<std::string>();
            rhs.MeshGuid = node["mesh"].as<std::string>();
            return true;
        }
    };
}
