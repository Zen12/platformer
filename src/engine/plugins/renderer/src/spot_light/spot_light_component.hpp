#pragma once

#include "spot_light_component_serialization.hpp"
#include "glm/mat4x4.hpp"

class SpotLightComponent final
{
public:
    const SpotLightComponentSerialization LightData{};
    glm::mat4 Projection{1.0f};
    glm::mat4 View{1.0f};

    explicit SpotLightComponent(SpotLightComponentSerialization data)
        : LightData(std::move(data))
    {}
};
