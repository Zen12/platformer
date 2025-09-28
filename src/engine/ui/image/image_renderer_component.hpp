#pragma once

#include "../../renderer/mesh/mesh.hpp"
#include "../../renderer/material.hpp"
#include "../../components/entity.hpp"
#include "../rect_transform/rect_transform_component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class UiImageRendererComponent final : public Component
{
private:
    Mesh _mesh;
    std::weak_ptr<Material> _material{};
    std::weak_ptr<Sprite> _sprite{};
    float _fillAmount{1.0f};
    glm::vec4 _tint{1.0f, 1.0f, 1.0f, 1.0f};

public:
    explicit UiImageRendererComponent(const std::weak_ptr<Entity> &entity);

    void SetMaterial(const std::weak_ptr<Material> &material) { _material = material; }
    void SetFillAmount(const float& amount) { _fillAmount = amount; }
    void SetTint(const glm::vec4& color) { _tint = color; }
    void SetSprite(const std::weak_ptr<Sprite> &sprite) { _sprite = sprite; }

    void Update([[maybe_unused]] const float& deltaTime) override;
    void Render(const glm::mat4 &projection) const;

    [[nodiscard]] int32_t GetShaderId() const { return _material.lock()->GetShaderId(); }
};