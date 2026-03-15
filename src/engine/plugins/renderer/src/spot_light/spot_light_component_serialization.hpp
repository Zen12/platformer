#pragma once

#include "entity/component_serialization.hpp"
#include <glm/glm.hpp>

struct SpotLightComponentSerialization final : public ComponentSerialization
{
    float InnerAngle{20.0f};
    float OuterAngle{35.0f};
    float Range{30.0f};
    float Intensity{1.5f};
    glm::vec3 Color{1.0f, 0.9f, 0.8f};
    float NearPlane{0.1f};
    float FarPlane{30.0f};
};
