#pragma once

#include "../render/font.hpp"
#include "../render/mesh.hpp"
#include "../render/material.hpp"
#include "transforms/transform.hpp"
#include "transforms/rect_transform.hpp"
#include "entity.hpp"

class UiImageRenderer final : public Component
{
private:
    Mesh _mesh;
    std::weak_ptr<Material> _material;

public:
    UiImageRenderer() = delete;

    explicit UiImageRenderer(const std::weak_ptr<Entity> &entity)
        : Component(entity),
          _mesh(Mesh::GenerateUI())
    {
    }

    void Update() const override;

    void SetMaterial(const std::weak_ptr<Material> &material)
    {
        _material = material;
    }

    void SetSprite(const std::weak_ptr<Sprite> &sprite) const;
    void Render(const glm::mat4 &projection) const;
};