#pragma once
#include "glm/vec2.hpp"


class BoxColliderComponent final {
public:
    uint32_t Id = 0;
    glm::vec2 Extents{};

    explicit BoxColliderComponent(const Box2dColliderSerialization &boxColliderSerialization)
        : Extents(boxColliderSerialization.scale)
    {}
};
