#pragma once
#include "../../asset/serialization/serialization_component.hpp"
#include <glm/vec3.hpp>

struct Light2dComponentSerialization final : public ComponentSerialization
{
    std::string CenterTransform;
    glm::vec3 Color;
    glm::vec3 Offset;

    float Radius = 20;
    int Segments = 500;

    float MaxAngle = 360;
    float StartAngle = 0;
};