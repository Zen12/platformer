#pragma once

#include "../../asset/serialization/serialization_component.hpp"

struct UiImageComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGuid;
    std::string SpriteGuid;
    float FillAmount{1};
};