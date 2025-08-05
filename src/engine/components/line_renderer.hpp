#pragma once
#include "entity.hpp"
#include "../render/material.hpp"
#include "../render/line.hpp"


class LineRenderer final: public Component {
    Line _line;
    std::weak_ptr<Material> _material;

public:

    LineRenderer() = delete;
    explicit LineRenderer(const std::weak_ptr<Entity> &entity)
    : Component(entity),
      _line(Line::Generate())
    {}

    void Update([[maybe_unused]] const float& deltaTime) const override {
        if (const auto material = _material.lock()) {
            material->Bind();
        }
    }

    void Render() const noexcept {
        _line.Bind();
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

    void SetPosition(const glm::vec3& a, const glm::vec3& b) {
        _line.UpdateVertices(std::vector<float>{a.x, a.y, a.z, b.x, b.y, b.z});
    }

};