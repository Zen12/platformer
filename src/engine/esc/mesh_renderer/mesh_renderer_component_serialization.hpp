#pragma once
#include <optional>
#include <glm/vec4.hpp>
#include "entity/component_serialization.hpp"
#include "guid.hpp"

struct MeshRendererComponentSerialization final : public ComponentSerialization
{
    Guid MeshGuid;
    Guid MaterialGuid;
    std::optional<glm::vec4> Color;
};
