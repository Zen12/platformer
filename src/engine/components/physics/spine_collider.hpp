#pragma once

#include "../renderering/spine_renderer.hpp"
#include "../../physics/physics_world.hpp"


class SpineColliderComponent final : public Component {
private:
    std::weak_ptr<SpineRenderer> _renderer;
    std::weak_ptr<PhysicsWorld> _physicsWorld;
    std::weak_ptr<Rigidbody2dComponent> _rigidbody2d;

    std::unordered_map<spine::Bone *, std::weak_ptr<BoxCollider2DComponent>> _bonesColliders{};

private:
    void CreateColliderFixture(const std::weak_ptr<BoxCollider2DComponent> &collider) const;
    [[nodiscard]] std::weak_ptr<BoxCollider2DComponent> CreateCollider() const;

public:
    explicit SpineColliderComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetPhysicsWorld(const std::weak_ptr<PhysicsWorld> &physicsWorld) noexcept;

    void SetRenderer(const std::weak_ptr<SpineRenderer> &renderer) noexcept;

    void SetRigidBody2d(const std::weak_ptr<Rigidbody2dComponent> &rigidbody2d) noexcept;

    void Update(const float &deltaTime) override;
};
