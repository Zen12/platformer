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
#include <vector>
#include <string>
#include <ranges>
#include <iostream>
#include "../components/renderering/sprite_renderer.hpp"
#include "../components/transforms/transform.hpp"
#include "../debug/debug.hpp"


class RayCastClosestCallback final : public b2RayCastCallback {
private:
    std::function<bool(b2Fixture*)> _filterFn;

public:
    b2Vec2 Point;
    b2Vec2 Normal;
    float Fraction = 1.0f;
    bool Hit = false;
    b2Fixture *Fixture;
    using FilterFn = std::function<bool(b2Fixture*)>;


    void SetFilter(const std::function<bool(b2Fixture*)> &filter) noexcept{
        _filterFn = filter;
    }

    float ReportFixture(b2Fixture* fixture, const b2Vec2& p,
                        const b2Vec2& n, const float f) override {

        // If filter exists and returns false -> skip fixture
        if (_filterFn && !_filterFn(fixture)) {
            return -1.0f; // ignore and continue
        }

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
    struct WeakPtrHash {
        template <typename T>
        std::size_t operator()(const std::weak_ptr<T>& wp) const noexcept {
            auto sp = wp.lock();
            return std::hash<T*>()(sp.get()); // hash the raw pointer
        }
    };

    struct WeakPtrEqual {
        template <typename T>
        bool operator()(const std::weak_ptr<T>& a, const std::weak_ptr<T>& b) const noexcept {
            return !a.owner_before(b) && !b.owner_before(a); // compare owner
        }
    };

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

    std::unordered_map<std::weak_ptr<BoxCollider2DComponent>, std::weak_ptr<Rigidbody2dComponent>, WeakPtrHash, WeakPtrEqual> _colliderToRigidBody{};
    std::unordered_map<std::weak_ptr<BoxCollider2DComponent>, b2Fixture*, WeakPtrHash, WeakPtrEqual> _boxColliderToFixture;
    std::unordered_map< b2Fixture*,std::weak_ptr<BoxCollider2DComponent>> _fixtureToCollider;

    std::unordered_map<std::weak_ptr<Rigidbody2dComponent>, b2Body*,  WeakPtrHash, WeakPtrEqual> _bodies{};
    RayCastClosestCallback _callback{};
    RayCastResult _result{};

public:
    explicit PhysicsWorld(b2Vec2 gravity)
        : _world(new b2World(gravity))
    {}

    void Simulate(const float& deltaTime) const;

    void UpdateRigidBodies() const;

    void UpdateColliders(const float& deltaTime) const;

    [[nodiscard]] const RayCastResult RayCast(const glm::vec3 &origin, const glm::vec3 &target);
    [[nodiscard]] const RayCastResult RayCast(const glm::vec3& origin, const glm::vec3& target, const int &ignoreLayers);

    [[nodiscard]] std::weak_ptr<b2World> GetWorld() const;

    void AddColliderComponent(
        const std::weak_ptr<Rigidbody2dComponent> &rigidBody,
        const std::weak_ptr<BoxCollider2DComponent> &collider,
        const b2FixtureDef &fixtureDef);

    [[nodiscard]] b2Fixture* GetFixtureByCollider(const std::weak_ptr<BoxCollider2DComponent> &collider) const noexcept;

    [[nodiscard]] b2Body *GetBody(const std::weak_ptr<Rigidbody2dComponent> &rigidBody) const noexcept;

    void AddRigidBodyComponent(const std::weak_ptr<Rigidbody2dComponent> &rigidBody, b2Body *body);

    void RemoveRigidBody(const std::weak_ptr<Rigidbody2dComponent> &rigidBody);
    void RemoveCollider(const std::weak_ptr<BoxCollider2DComponent> &collider);
};
