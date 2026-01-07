#pragma once

#include "../../entity/component_serialization.hpp"
#include <glm/glm.hpp>
#include <string>

struct DirectionalLightComponentSerialization final : public ComponentSerialization
{
    float AspectRatio{1.0f};
    float NearPlane{0.1f};
    float FarPlane{100.0f};
    float OrthoSize{20.0f};
    std::string FollowTag{};
    glm::vec3 FollowOffset{0.0f, 30.0f, 0.0f};
};
