#pragma once
#include <yaml-cpp/yaml.h>
#include "gen_mesh_renderer_component_serialization.hpp"
#include "guid/guid_yaml.hpp"

namespace YAML {
    template <>
    struct convert<GenMeshRendererComponentSerialization> {
        static bool decode(const Node& node, GenMeshRendererComponentSerialization& rhs) {
            rhs.MaterialGuid = node["material"].as<Guid>();
            rhs.MeshGuid = node["mesh"].as<Guid>();
            if (node["color"]) {
                const auto colorNode = node["color"];
                rhs.Color = glm::vec4(
                    colorNode[0].as<float>(),
                    colorNode[1].as<float>(),
                    colorNode[2].as<float>(),
                    colorNode[3].as<float>()
                );
            }
            return true;
        }
    };
}
