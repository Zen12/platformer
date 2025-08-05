#pragma once
#include <iostream>
#include <box2d/box2d.h>

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

    void Simulate(const float& deltaTime) const {
        _world->Step(deltaTime, velocityIterations, positionIterations);
    }

    void UpdateColliders(const float& deltaTime) const {
        for (const auto& component : _components) {
            if (auto collider = component.lock()) {
                collider->Update(deltaTime);
            }
        }
    }

    [[nodiscard]] std::weak_ptr<b2World> GetWorld() const {
        return _world;
    }

    void AddColliderComponent(const std::weak_ptr<Component> &collider) {
        _components.push_back(collider);
    }

};