#pragma once

#include "../../asset/serialization/serialization_component.hpp"

struct GridSerialization final : public ComponentSerialization {
    std::vector<std::vector<int>> grid;
    glm::vec3 spawnOffset;
    glm::vec3 spawnStep;
};