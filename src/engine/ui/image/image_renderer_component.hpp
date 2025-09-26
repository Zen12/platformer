#pragma once

#include "../../render/font.hpp"
#include "../../render/mesh.hpp"
#include "../../render/material.hpp"
#include "../../components/transforms/transform.hpp"
#include "../../components/transforms/rect_transform.hpp"
#include "../../components/entity.hpp"

class UiImageRendererComponent final : public Component
{
private:
    Mesh _mesh;
    std::weak_ptr<Material> _material;
    std::weak_ptr<Sprite> _sprite;
    float _fillAmount{1};
    glm::vec4 _tint{1, 1, 1, 1};

public:
    UiImageRendererComponent() = delete;

    explicit UiImageRendererComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity),
          _mesh(Mesh::GenerateUI())
    {
    }

    void Update([[maybe_unused]] const float& deltaTime) override;

    void SetMaterial(const std::weak_ptr<Material> &material)
    {
        _material = material;
    }

    void SetFillAmount(const float& amount) noexcept;
    void SetTint(const glm::vec4& color);
    void SetSprite(const std::weak_ptr<Sprite> &sprite) noexcept;
    void Render(const glm::mat4 &projection) const;
};