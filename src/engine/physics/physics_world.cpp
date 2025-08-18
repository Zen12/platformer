#include "physics_world.hpp"

#include "../components/transforms/transform.hpp"

void PhysicsWorld::Simulate(const float &deltaTime) const {
    _world->Step(deltaTime, velocityIterations, positionIterations);
    UpdateRigidBodies();
    UpdateColliders(deltaTime);
}

void PhysicsWorld::UpdateRigidBodies() const {
    for (auto && body: _bodies) {
        if (const auto rig = body.first.lock()) {
            const auto pos = rig->GetEntity().lock()->GetComponent<Transform>().lock()->GetPosition();
            body.second->SetTransform(b2Vec2(pos.x, pos.y), 0.0f);
        }
    }
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
    return RayCast(origin, target, std::vector<std::string>{});
}

RayCastResult PhysicsWorld::RayCast(
    const glm::vec3 &origin,
    const glm::vec3 &target,
    const std::vector<std::string> &ignoreTags) {

    b2Vec2 pointA(origin.x, origin.y);    // Start of ray
    b2Vec2 pointB(target.x, target.y);   // End of ray

    RayCastClosestCallback callback;

    if (!ignoreTags.empty()) {
        RayCastClosestCallback::FilterFn filter = [this, ignoreTags](b2Fixture* fixture) {
            for (const auto& [rig, fix] : _fixtures ) {
                if (fix == fixture) {
                    if (const auto boxComponent = rig.lock()) {
                        if (const auto entity = boxComponent->GetEntity().lock()) {
                            return !(std::find(ignoreTags.begin(), ignoreTags.end(), entity->GetTag()) != ignoreTags.end());
                        }
                    }
                }
            }

            return true;
        };

        callback.SetFilter(filter);
    }


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
