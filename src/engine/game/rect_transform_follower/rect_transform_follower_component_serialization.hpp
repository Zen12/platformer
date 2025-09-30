#pragma once
#include "../../entity/component_serialization.hpp"
#include <glm/vec2.hpp>

struct RectTransformFollowerSerialization final : public ComponentSerialization {
    bool UseCreator;
    std::string Target;
    glm::vec2 Offset{};
};