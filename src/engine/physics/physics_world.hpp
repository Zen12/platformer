#pragma once
#include <iostream>
#include <map>
#include <box2d/box2d.h>
#include "../components/entity.hpp"
#include "../components/physics/rigidbody2d_component.hpp"
#include "../components/physics/box_collider2d_component.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include "glm/ext/scalar_int_sized.hpp"

class RayCastClosestCallback final : public b2RayCastCallback {
public:
    b2Vec2 Point;
    b2Vec2 Normal;
    float Fraction = 1.0f;
    bool Hit = false;
    b2Fixture *Fixture;

    float ReportFixture(b2Fixture* fixture, const b2Vec2& p,
                        const b2Vec2& n, const float f) override {
        Hit = true;
        Point = p;
        Normal = n;
        Fraction = f;
        Fixture = fixture;
        return f; // return the fraction to clip the ray and get closest hit
    }
};

struct RayCastResult {
    bool IsHit;
    glm::vec3 Point;
    glm::vec3 Normal;
    std::weak_ptr<Rigidbody2dComponent> Rigidbody;
    std::weak_ptr<BoxCollider2DComponent> BoxCollider;
};

class PhysicsWorld
{
    struct WeakPtrCompare {
        template <typename T>
        bool operator()(const std::weak_ptr<T>& lhs, const std::weak_ptr<T>& rhs) const {
            return lhs.owner_before(rhs);
        }
    };
private:
    std::shared_ptr<b2World> _world{};
    const glm::int32 velocityIterations = 6;
    const glm::int32 positionIterations = 2;
    std::map<std::weak_ptr<Rigidbody2dComponent>, std::vector<std::weak_ptr<BoxCollider2DComponent>>, WeakPtrCompare> _colliders{};
    std::map<std::weak_ptr<BoxCollider2DComponent>, std::weak_ptr<Rigidbody2dComponent>, WeakPtrCompare> _rigidBodies{};
    std::map<std::weak_ptr<BoxCollider2DComponent>, b2Fixture*, WeakPtrCompare> _fixtures{};
    std::map<std::weak_ptr<Rigidbody2dComponent>, b2Body*, WeakPtrCompare> _bodies{};

public:
    explicit PhysicsWorld(b2Vec2 gravity)
        : _world(new b2World(gravity))
    {}

    void Simulate(const float& deltaTime) const;

    void UpdateColliders(const float& deltaTime) const;

    [[nodiscard]] RayCastResult RayCast(const glm::vec3& origin, const glm::vec3& direction);

    [[nodiscard]] std::weak_ptr<b2World> GetWorld() const;

    void AddColliderComponent(
        const std::weak_ptr<Rigidbody2dComponent> &rigidBody,
        const std::weak_ptr<BoxCollider2DComponent> &collider,
        const b2FixtureDef &fixtureDef);

    void AddRigidBodyComponent(const std::weak_ptr<Rigidbody2dComponent> &rigidBody, b2Body *body);

    void RemoveRigidBody(const std::weak_ptr<Rigidbody2dComponent> &rigidBody);
};
