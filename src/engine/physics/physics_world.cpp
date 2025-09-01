#include "physics_world.hpp"

#define DEBUG_ENGINE_RENDER_COLLIDERS 1
#define DEBUG_ENGINE_PHYSICS_PROFLE 1

void PhysicsWorld::Simulate(const float &deltaTime) const {
    _world->Step(deltaTime, velocityIterations, positionIterations);
    UpdateRigidBodies();
    UpdateColliders(deltaTime);
}

void PhysicsWorld::UpdateRigidBodies() const {
#ifndef NDEBUG
#if DEBUG_ENGINE_RENDER_COLLIDERS
    PROFILE_SCOPE("  PhysicsWorld::Update");
#endif
#endif

    for (auto &&body: _bodies) {
        if (const auto rig = body.first.lock()) {
            const auto pos = rig->GetEntity().lock()->GetComponent<Transform>().lock()->GetPosition();
            body.second->SetTransform(b2Vec2(pos.x, pos.y), 0.0f);
        }
    }
#ifndef NDEBUG
#if DEBUG_ENGINE_RENDER_COLLIDERS
    for (auto && [_, fixture]: _fixtures) {

        auto poly = dynamic_cast<b2PolygonShape*>(fixture->GetShape());

        const b2Transform& xf = fixture->GetBody()->GetTransform();

        b2Vec2 previous = b2Mul(xf, poly->m_vertices[0]);
        for (int i = 1; i < poly->m_count; i++)
        {
            b2Vec2 worldVertex = b2Mul(xf, poly->m_vertices[i]);
            DebugLines::AddLine(glm::vec2(previous.x, previous.y), glm::vec2(worldVertex.x, worldVertex.y));
            previous = worldVertex;
        }

        b2Vec2 first = b2Mul(xf, poly->m_vertices[0]);
        DebugLines::AddLine(glm::vec2(previous.x, previous.y), glm::vec2(first.x, first.y));

    }
#endif
#endif

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

const RayCastResult PhysicsWorld::RayCast(const glm::vec3 &origin, const glm::vec3 &target) {
    b2Vec2 pointA(origin.x, origin.y);    // Start of ray
    b2Vec2 pointB(target.x, target.y);   // End of ray

    _callback.Hit = false;
    _callback.SetFilter(nullptr);

    _world->RayCast(&_callback, pointA, pointB);

    _result.IsHit =_callback.Hit;

    if (_callback.Hit) {
        _result.Point = glm::vec3(_callback.Point.x, _callback.Point.y, 0.0f);
        _result.Normal = glm::vec3(_callback.Normal.x, _callback.Normal.y, 0.0f);
    }

    for (const auto& component : _colliders) {
        const auto pair = component.second;
        for (auto& collider : pair) {
            if (_fixtures[collider] == _callback.Fixture) {
                _result.Rigidbody = _rigidBodies[collider];
                _result.BoxCollider = collider;
                return _result;
            }
        }
    }

    return _result;
}

const RayCastResult PhysicsWorld::RayCast(
    const glm::vec3 &origin,
    const glm::vec3 &target,
    const std::vector<std::string> &ignoreTags) {

    b2Vec2 pointA(origin.x, origin.y);    // Start of ray
    b2Vec2 pointB(target.x, target.y);   // End of ray

    _callback.Hit = false;

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

    } else {
        _callback.SetFilter(nullptr);
    }


    _world->RayCast(&_callback, pointA, pointB);

    _result.IsHit =_callback.Hit;

    if (_callback.Hit) {
        _result.Point = glm::vec3(_callback.Point.x, _callback.Point.y, 0.0f);
        _result.Normal = glm::vec3(_callback.Normal.x, _callback.Normal.y, 0.0f);
    }

    for (const auto& component : _colliders) {
        const auto pair = component.second;
        for (auto& collider : pair) {
            if (_fixtures[collider] == _callback.Fixture) {
                _result.Rigidbody = _rigidBodies[collider];
                _result.BoxCollider = collider;
                return _result;
            }
        }
    }

    return _result;
}

std::weak_ptr<b2World> PhysicsWorld::GetWorld() const {
    return _world;
}

void PhysicsWorld::AddColliderComponent(
    const std::weak_ptr<Rigidbody2dComponent> &rigidBody,
    const std::weak_ptr<BoxCollider2DComponent> &collider,
    const b2FixtureDef &fixtureDef)
{
    const auto body = _bodies.at(rigidBody);
    const auto fixture = body->CreateFixture(&fixtureDef);

    if (auto& vec = _colliders[rigidBody];
        std::find_if(vec.begin(), vec.end(),
                     [&](const std::weak_ptr<BoxCollider2DComponent>& c) {
                         return !c.expired() && !collider.expired() &&
                                c.lock().get() == collider.lock().get();
                     }) == vec.end())
    {
        vec.push_back(collider);
    }

    _rigidBodies[collider] = rigidBody;
    _fixtures[collider] = fixture;
}

[[nodiscard]] b2Fixture* PhysicsWorld::GetFixtureByCollider(const std::weak_ptr<BoxCollider2DComponent> &collider) const noexcept {
    return _fixtures.at(collider);
}

[[nodiscard]] b2Body* PhysicsWorld::GetBody(const std::weak_ptr<Rigidbody2dComponent> &rigidBody) const noexcept {
    return _bodies.at(rigidBody);
}

void PhysicsWorld::AddRigidBodyComponent(const std::weak_ptr<Rigidbody2dComponent> &rigidBody, b2Body *body) {

    if (_colliders.find(rigidBody) == _colliders.end()) {
        _colliders[rigidBody] = std::vector<std::weak_ptr<BoxCollider2DComponent>>();
        _bodies[rigidBody] = body;
    }
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
