#pragma once
#include "../entity/component_serialization.hpp"

struct PathMoverComponentSerialization final : public ComponentSerialization {
    std::vector<glm::vec3> Positions;
    float Speed;
};