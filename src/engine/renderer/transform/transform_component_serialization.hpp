#pragma once
#include "../../asset/serialization/serialization_component.hpp"
#include <glm/vec3.hpp>

struct TransformComponentSerialization final : public ComponentSerialization
{
    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 scale{};
};