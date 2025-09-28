#pragma once
#include "../rigidbody/rigidbody2d_component.hpp"


class BoxCollider2DComponent final : public Component
{

public:
    explicit BoxCollider2DComponent(const std::weak_ptr<Entity> &entity) : Component(entity)
    {

    }

public:

    void Update([[maybe_unused]] const float& deltaTime) override
    {
    }
};