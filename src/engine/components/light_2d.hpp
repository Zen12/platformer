#pragma once
#include <utility>

#include "entity.hpp"
#include "mesh_renderer.hpp"
#include "../render/material.hpp"
#include "../physics/physics_world.hpp"
#include "../physics/raycast.hpp"
#include "../render/line.hpp"
#include "transforms/transform.hpp"


class Light2dComponent final : public Component {
    std::weak_ptr<Transform> _center;
    std::weak_ptr<Material> _material;

    std::weak_ptr<PhysicsWorld> _physicsWorld;
    std::weak_ptr<MeshRenderer> _meshRenderer;

    const float radius = 20.0f;
    const int segments = 50;



public:

    Light2dComponent() = delete;
    explicit Light2dComponent(const std::weak_ptr<Entity> &entity)
    : Component(entity) {
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
                std::vector<float> meshVert{};
                std::vector<unsigned int> meshIndex{};

                const auto centerPosition = center->GetPosition();

                meshVert.emplace_back(centerPosition.x);
                meshVert.emplace_back(centerPosition.y);
                meshVert.emplace_back(0);

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

                    if (callback.Hit) {
                        x = callback.Point.x;
                        y = callback.Point.y;
                    }

                    const auto vertex = std::vector<float>{
                        centerPosition.x, centerPosition.y, 0.0f,
                        x,y, 0.0f};

                    meshVert.emplace_back(x);
                    meshVert.emplace_back(y);
                    meshVert.emplace_back(0);

                }

                for (int j = 1; j < segments; j++) {
                    meshIndex.emplace_back(0);
                    meshIndex.emplace_back(j);
                    meshIndex.emplace_back(j+1);
                }

                meshIndex.emplace_back(0);
                meshIndex.emplace_back(segments);
                meshIndex.emplace_back(1);

                if (const auto &meshRenderer = _meshRenderer.lock()) {
                    meshRenderer->SetUniformVec3("center", centerPosition);
                    meshRenderer->UpdateMesh(meshVert, meshIndex);
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

    void SetMeshRenderer(std::weak_ptr<MeshRenderer> meshRenderer) noexcept {
        _meshRenderer = std::move(meshRenderer);
    }

    void SetCenterTransform(std::weak_ptr<Transform> center) noexcept {
        _center = std::move(center);
    }

    void SetPhysicsWorld(std::weak_ptr<PhysicsWorld> physicsWorld) noexcept {
        _physicsWorld = std::move(physicsWorld);
    }

};
