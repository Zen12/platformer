#pragma once
#include "../../asset/serialization/serialization_component.hpp"

struct MeshRendererComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGuid;
};