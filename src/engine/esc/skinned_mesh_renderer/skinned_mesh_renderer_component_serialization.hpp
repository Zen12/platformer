#pragma once
#include "entity/component_serialization.hpp"
#include "guid.hpp"

struct SkinnedMeshRendererComponentSerialization final : public ComponentSerialization
{
    Guid MeshGuid;
    Guid MaterialGuid;
};
