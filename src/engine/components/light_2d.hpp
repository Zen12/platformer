#pragma once
#include <utility>

#include "entity.hpp"
#include "mesh_renderer.hpp"
#include "../render/material.hpp"
#include "../physics/physics_world.hpp"
#include "transforms/transform.hpp"


class Light2dComponent final : public Component {
private:
    std::weak_ptr<Transform> _center;

    std::weak_ptr<PhysicsWorld> _physicsWorld;
    std::weak_ptr<MeshRenderer> _meshRenderer;

    const float _radius = 20.0f;
    const int _segments = 50;

    glm::vec3 _offset =  glm::vec3{0.0f, 2.0f, 0.0f};



public:

    Light2dComponent() = delete;
    explicit Light2dComponent(const std::weak_ptr<Entity> &entity) : Component(entity) {
    }

    void Update([[maybe_unused]] const float& deltaTime) override;

    void SetMeshRenderer(std::weak_ptr<MeshRenderer> meshRenderer) noexcept {
        _meshRenderer = std::move(meshRenderer);
    }

    void SetCenterTransform(std::weak_ptr<Transform> center) noexcept {
        _center = std::move(center);
    }

    void SetPhysicsWorld(std::weak_ptr<PhysicsWorld> physicsWorld) noexcept {
        _physicsWorld = std::move(physicsWorld);
    }

    void SetOffset(const glm::vec3 &offset) noexcept {
        _offset = offset;
    }

private:

    // move to utils class
    [[nodiscard]] static float DegToRad(const float& degrees) noexcept {
        return M_PI / 180.0f * degrees;
    }
};
