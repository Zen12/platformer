#pragma once
#include <yaml-cpp/yaml.h>
#include "skinned_mesh_renderer_component_serialization.hpp"
#include "../../system/guid_yaml.hpp"

namespace YAML
{
    template <>
    struct convert<SkinnedMeshRendererComponentSerialization>
    {
        static bool decode(const Node &node, SkinnedMeshRendererComponentSerialization &rhs)
        {
            rhs.MaterialGuid = node["material"].as<Guid>();
            rhs.MeshGuid = node["mesh"].as<Guid>();
            return true;
        }
    };
}
