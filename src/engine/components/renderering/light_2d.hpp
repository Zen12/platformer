#pragma once
#include <utility>

#include "../entity.hpp"
#include "mesh_renderer.hpp"
#include "../../render/material.hpp"
#include "../../physics/physics_world.hpp"
#include "../transforms/transform.hpp"


class Light2dSettings {
    public:
    float Radius = 1;
    int Segments = 500;

    glm::vec3 Color = glm::vec3(1, 1, 1);
    glm::vec3 Offset = glm::vec3{0.0f, 2.0f, 0.0f};

    float MaxAngle = 360;
    float StartAngle = 0;
};


class Light2dComponent final : public Component {
private:
    std::weak_ptr<Transform> _center;

    std::weak_ptr<PhysicsWorld> _physicsWorld;
    std::weak_ptr<MeshRenderer> _meshRenderer;

    Light2dSettings _settings{};

    //int FindNexClosesVertex(b2Fixture* fixture, b2PolygonShape *shape, glm::vec2 center, glm::vec2 point);

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

    void SetSettings(const Light2dSettings &settings) noexcept {
        _settings = settings;
    }

private:

    // move to utils class
    [[nodiscard]] static float DegToRad(const float& degrees) noexcept {
        return M_PI / 180.0f * degrees;
    }
};
