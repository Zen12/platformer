#pragma once
#include <optional>
#include <glm/vec4.hpp>
#include "gen_mesh_renderer_component_serialization.hpp"
#include "bounds.hpp"
#include "guid/guid.hpp"

struct GenMeshRendererComponent final {
    Guid MeshGuid;
    Guid MaterialGuid;
    Bounds MeshBounds;
    std::optional<glm::vec4> Color;

    explicit GenMeshRendererComponent(const GenMeshRendererComponentSerialization& serialization)
        : MeshGuid(serialization.MeshGuid), MaterialGuid(serialization.MaterialGuid), Color(serialization.Color) {}

    GenMeshRendererComponent(const Guid& meshGuid, const Guid& materialGuid, const std::optional<glm::vec4>& color = std::nullopt)
        : MeshGuid(meshGuid), MaterialGuid(materialGuid), Color(color) {}
};
