#pragma once
#include <cstdint>
#include <string>

#include "mesh_renderer_component_serialization.hpp"
#include "../../renderer/bounds.hpp"


class MeshRendererComponent final {
public:
    std::string Guid;
    std::string MaterialGuid;
    Bounds MeshBounds;  // Populated on first render from Scene

    explicit MeshRendererComponent(const MeshRendererComponentSerialization &serialization)
        : Guid(serialization.MeshGuid), MaterialGuid(serialization.MaterialGuid) {}
};
