#pragma once
#include <utility>

#include "entity.hpp"
#include "../render/material.hpp"
#include "../physics/physics_world.hpp"
#include "../physics/raycast.hpp"
#include "../render/line.hpp"
#include "transforms/transform.hpp"


class Light2dComponent final : public Component {
    std::weak_ptr<Transform> _center;
    std::weak_ptr<Material> _material;
    std::vector<std::unique_ptr<Line>> _lines;
    std::weak_ptr<PhysicsWorld> _physicsWorld;

    const float radius = 20.0f;
    const int segments = 50;



public:

    Light2dComponent() = delete;
    explicit Light2dComponent(const std::weak_ptr<Entity> &entity)
    : Component(entity) {
        for (int i = 0;i < segments; i++) {
            _lines.emplace_back(std::make_unique<Line>(std::vector<float>{0,200,0,0,200,0}));
        }
    }

    void Update() const override {
        if (const auto material = _material.lock()) {
            material->Bind();
        }
    }
    // Convert degrees to radians
    [[nodiscard]] static float DegToRad(const float& degrees) noexcept {
        return M_PI / 180.0f * degrees;
    }

    void Render() noexcept {

        if (const auto world = _physicsWorld.lock()->GetWorld().lock()) {
            if (const auto center = _center.lock()) {
                const auto centerPosition = center->GetPosition();

                const float step = 360.0f / static_cast<float>(segments);

                for (int i = 0; i < segments; i++) {
                    const float angle = static_cast<float>(i) * step;

                    const float angleRad = DegToRad(angle);
                    float x = centerPosition.x + radius * std::cos(angleRad);
                    float y = centerPosition.y + radius * std::sin(angleRad);

                    b2Vec2 pointA(centerPosition.x, centerPosition.y);    // Start of ray
                    b2Vec2 pointB(x, y);   // End of ray

                    RayCastClosestCallback callback;
                    world->RayCast(&callback, pointA, pointB);

                    if (callback.hit) {
                        x = callback.point.x;
                        y = callback.point.y;
                    }

                    const auto vertex = std::vector<float>{
                        centerPosition.x, centerPosition.y, 0.0f,
                        x,y, 0.0f};

                    _lines[i]->UpdateVertices(vertex);
                    _lines[i]->Bind();
                }
            }
        }
    }


    [[nodiscard]] int32_t GetShaderId() const noexcept {
        if (const auto mat =  _material.lock()) {
            return mat->GetShaderId();
        }
        return -1;
    }

    void SetMaterial(std::weak_ptr<Material> material) noexcept {
        _material = std::move(material);
    }

    void SetCenterTransform(std::weak_ptr<Transform> center) noexcept {
        _center = std::move(center);
    }

    void SetPhysicsWorld(std::weak_ptr<PhysicsWorld> physicsWorld) noexcept {
        _physicsWorld = std::move(physicsWorld);
    }

};
