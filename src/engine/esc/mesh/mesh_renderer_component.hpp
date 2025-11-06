#pragma once
#include <cstdint>
#include <string>

#include "../../renderer/mesh/mesh_renderer_component_serialization.hpp"


class MeshRendererComponent final {
public:
    std::string Guid;
    std::string MaterialGuid;

    explicit MeshRendererComponent(const MeshRendererComponentSerialization &serialization)
        : Guid(serialization.MeshGuid), MaterialGuid(serialization.MaterialGuid) {}
};
