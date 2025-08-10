#include "physics_world.hpp"

void PhysicsWorld::Simulate(const float &deltaTime) const {
    _world->Step(deltaTime, velocityIterations, positionIterations);
}

void PhysicsWorld::UpdateColliders(const float &deltaTime) const {
    for (const auto& component : _components) {
        if (auto collider = component.lock()) {
            collider->Update(deltaTime);
        }
    }
}

std::weak_ptr<b2World> PhysicsWorld::GetWorld() const {
    return _world;
}

void PhysicsWorld::AddColliderComponent(const std::weak_ptr<Component> &collider) {
    _components.push_back(collider);
}
