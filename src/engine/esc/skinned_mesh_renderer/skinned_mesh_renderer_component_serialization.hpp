#pragma once
#include "../../entity/component_serialization.hpp"

struct SkinnedMeshRendererComponentSerialization final : public ComponentSerialization
{
    std::string MeshGuid;
    std::string MaterialGuid;
};
