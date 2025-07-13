#pragma once
#include "entity.hpp"
#include "../render/material.hpp"
#include "../render/line.hpp"
#include "transforms/transform.hpp"


class Light2dComponent final : public Component {
    std::weak_ptr<Transform> _center;
    std::weak_ptr<Material> _material;
    std::vector<std::unique_ptr<Line>> _lines;

    const float radius = 1.0f;
    const int segments = 10;



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

        if (const auto center = _center.lock()) {
            const auto centerPosition = center->GetPosition();

            const float step = 360.0f / static_cast<float>(segments);

            for (int i = 0; i < segments; i++) {
                const float angle = static_cast<float>(i) * step;

                const float angleRad = DegToRad(angle);
                const float x = radius * std::cos(angleRad);
                const float y = radius * std::sin(angleRad);

                const auto vertex = std::vector<float>{
                    centerPosition.x, centerPosition.y, 0.0f,
                    centerPosition.x + x,centerPosition.y + y, 0.0f};

                _lines[i]->UpdateVertices(vertex);
                _lines[i]->Bind();
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

    void SetCenterTransform(std::weak_ptr<Transform> center) {
        _center = std::move(center);
    }

};
