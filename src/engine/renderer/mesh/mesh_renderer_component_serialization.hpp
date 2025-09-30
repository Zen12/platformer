#pragma once
#include "../../entity/component_serialization.hpp"

struct MeshRendererComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGuid;
};