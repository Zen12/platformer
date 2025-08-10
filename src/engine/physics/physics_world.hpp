#pragma once
#include <iostream>
#include <box2d/box2d.h>
#include "../components/entity.hpp"

#include "glm/ext/scalar_int_sized.hpp"

class PhysicsWorld
{
private:
    std::shared_ptr<b2World> _world{};
    const glm::int32 velocityIterations = 6;
    const glm::int32 positionIterations = 2;
    std::vector<std::weak_ptr<Component>> _components{};

public:
    explicit PhysicsWorld(b2Vec2 gravity)
        : _world(new b2World(gravity))
    {}

    void Simulate(const float& deltaTime) const;

    void UpdateColliders(const float& deltaTime) const;

    [[nodiscard]] std::weak_ptr<b2World> GetWorld() const;

    void AddColliderComponent(const std::weak_ptr<Component> &collider);
};
