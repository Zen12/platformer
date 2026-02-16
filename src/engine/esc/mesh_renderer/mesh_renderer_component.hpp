#pragma once
#include <cstdint>
#include <optional>
#include <glm/vec4.hpp>

#include "mesh_renderer_component_serialization.hpp"
#include "bounds.hpp"
#include "guid.hpp"


class MeshRendererComponent final {
public:
    Guid MeshGuid;
    Guid MaterialGuid;
    Bounds MeshBounds;
    std::optional<glm::vec4> Color;

    explicit MeshRendererComponent(const MeshRendererComponentSerialization &serialization)
        : MeshGuid(serialization.MeshGuid), MaterialGuid(serialization.MaterialGuid), Color(serialization.Color) {}

    MeshRendererComponent(const Guid& meshGuid, const Guid& materialGuid, const std::optional<glm::vec4>& color = std::nullopt)
        : MeshGuid(meshGuid), MaterialGuid(materialGuid), Color(color) {}
};
