#pragma once
#include "../../asset/serialization/serialization_component.hpp"
#include <string>

struct SpriteRenderComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGuid;
    std::string SpriteGuid;
};