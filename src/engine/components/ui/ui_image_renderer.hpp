#pragma once

#include "../../render/font.hpp"
#include "../../render/mesh.hpp"
#include "../../render/material.hpp"
#include "../transforms/transform.hpp"
#include "../transforms/rect_transform.hpp"
#include "../entity.hpp"

class UiImageRenderer final : public Component
{
private:
    Mesh _mesh;
    std::weak_ptr<Material> _material;
    std::weak_ptr<Sprite> _sprite;
    float _fillAmount{1};

public:
    UiImageRenderer() = delete;

    explicit UiImageRenderer(const std::weak_ptr<Entity> &entity)
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
    void SetSprite(const std::weak_ptr<Sprite> &sprite) noexcept;
    void Render(const glm::mat4 &projection) const;
};