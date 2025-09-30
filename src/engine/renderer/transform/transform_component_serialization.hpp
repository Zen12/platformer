#pragma once
#include "../../entity/component_serialization.hpp"
#include <glm/vec3.hpp>

struct TransformComponentSerialization final : public ComponentSerialization
{
    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 scale{};
};