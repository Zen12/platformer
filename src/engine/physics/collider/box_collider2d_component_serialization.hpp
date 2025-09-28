#pragma once
#include "../../asset/serialization/serialization_component.hpp"
#include <glm/vec3.hpp>

struct Box2dColliderSerialization final : public ComponentSerialization
{
    glm::vec3 scale{};
    glm::vec3 translate{};
};