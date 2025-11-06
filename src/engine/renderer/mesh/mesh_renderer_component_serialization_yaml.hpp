#pragma once
#include <yaml-cpp/yaml.h>
#include "mesh_renderer_component_serialization.hpp"
#include "../../system/guid_generator.hpp"

namespace YAML
{
    template <>
    struct convert<MeshRendererComponentSerialization>
    {
        static bool decode(const Node &node, MeshRendererComponentSerialization &rhs)
        {
            rhs.MaterialGuid = node["material"].as<std::string>();
            if (node["guid"])
                rhs.MeshGuid = node["guid"].as<std::string>();
            else
                rhs.MeshGuid = GuidGenerator::GenerateGuid();
            return true;
        }
    };
}