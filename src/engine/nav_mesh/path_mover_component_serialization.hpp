#pragma once
#include "../asset/serialization/serialization_component.hpp"

struct PathMoverComponentSerialization final : public ComponentSerialization {
    std::vector<glm::vec3> Positions;
    float Speed;
};