#pragma once

#include "directional_light_component_serialization.hpp"
#include "glm/mat4x4.hpp"

class DirectionalLightComponent final
{
public:
    const DirectionalLightComponentSerialization LightData{};
    glm::mat4 Projection{1.0f};
    glm::mat4 View{1.0f};

    explicit DirectionalLightComponent(DirectionalLightComponentSerialization data)
        : LightData(std::move(data))
    {}
};
