#pragma once
#include "../../entity/component_serialization.hpp"
#include <string>

struct SpriteRenderComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGuid;
    std::string SpriteGuid;
};