#pragma once

#include "../../entity/component_serialization.hpp"

struct UiImageComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGuid;
    std::string SpriteGuid;
    float FillAmount{1};
};