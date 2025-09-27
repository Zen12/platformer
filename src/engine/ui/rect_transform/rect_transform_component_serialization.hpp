#pragma once
#include "../../asset/serialization/serialization_component.hpp"
#include <vector>

struct RectLayoutSerialization
{
    const std::string Type;
    const float Value = 0;
    const float X = 0;
    const float Y = 0;

    RectLayoutSerialization(std::string type, const float value, const float &x, const float &y)
    : Type(std::move(type)), Value(value), X(x), Y(y)
    {
    }
};

struct RectTransformComponentSerialization final : public ComponentSerialization
{
    std::vector<RectLayoutSerialization> Layouts;
};