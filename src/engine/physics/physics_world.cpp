#include "physics_world.hpp"

void PhysicsWorld::Simulate(const float &deltaTime) const {
    _world->Step(deltaTime, velocityIterations, positionIterations);
}

void PhysicsWorld::UpdateColliders(const float &deltaTime) const {
    for (const auto& component : _colliders) {
        auto colliders = component.second;
        for (auto& collider : colliders) {
            if (auto col = collider.lock()) {
                col->Update(deltaTime);
            }
        }
    }
}

RayCastResult PhysicsWorld::RayCast(const glm::vec3 &origin, const glm::vec3 &target) {

    b2Vec2 pointA(origin.x, origin.y);    // Start of ray
    b2Vec2 pointB(target.x, target.y);   // End of ray

    RayCastClosestCallback callback;
    _world->RayCast(&callback, pointA, pointB);

    RayCastResult result{};

    if (callback.Hit) {
        result.IsHit = true;
        result.Point = glm::vec3(callback.Point.x, callback.Point.y, 0.0f);
        result.Normal = glm::vec3(callback.Normal.x, callback.Normal.y, 0.0f);
    }

    for (const auto& component : _colliders) {
        const auto pair = component.second;
        for (auto& collider : pair) {
            if (_fixtures[collider] == callback.Fixture) {
                result.Rigidbody = _rigidBodies[collider];
                result.BoxCollider = collider;
                return result;
            }
        }
    }

    return result;
}

std::weak_ptr<b2World> PhysicsWorld::GetWorld() const {
    return _world;
}

void PhysicsWorld::AddColliderComponent(
    const std::weak_ptr<Rigidbody2dComponent> &rigidBody,
    const std::weak_ptr<BoxCollider2DComponent> &collider,
    const b2FixtureDef &fixtureDef)
{
    const auto body = _bodies[rigidBody];
    const auto fixture = body->CreateFixture(&fixtureDef);

    _colliders[rigidBody].push_back(collider);
    _rigidBodies[collider] = rigidBody;
    _fixtures[collider] = fixture;
}

void PhysicsWorld::AddRigidBodyComponent(const std::weak_ptr<Rigidbody2dComponent> &rigidBody, b2Body *body) {

    _colliders[rigidBody] = std::vector<std::weak_ptr<BoxCollider2DComponent>>();
    _bodies[rigidBody] = body;
}

void PhysicsWorld::RemoveRigidBody(const std::weak_ptr<Rigidbody2dComponent> &rigidBody) {

    if (rigidBody.expired())
        return;

    _world->DestroyBody(_bodies[rigidBody]);

    const auto colliders = _colliders[rigidBody];
    for (auto& collider : colliders) {
        _rigidBodies.erase(collider);
        _fixtures.erase(collider);
    }

    _colliders.erase(rigidBody);
    _bodies.erase(rigidBody);

}
