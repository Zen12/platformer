#pragma once

#include "entity.hpp"
#include "../asset/asset_loader.hpp"
#include "../render/mesh.hpp"
#include "../render/material.hpp"

class SpriteRenderer : public Component
{

private:
    std::weak_ptr<Sprite> _sprite;
    Mesh _mesh;
    std::weak_ptr<Material> _material;

public:
    SpriteRenderer() = delete;
    explicit SpriteRenderer(const std::weak_ptr<Entity> &entity)
    : Component(entity),
      _mesh(Mesh::GenerateSprite())
    {}

    void Update([[maybe_unused]] const float& deltaTime) const override;

    void SetSprite(std::weak_ptr<Sprite> sprite) noexcept;

    void SetMaterial(std::weak_ptr<Material> material) noexcept;


    void Render() const noexcept;

    [[nodiscard]] int32_t GetShaderId() const noexcept;

    ~SpriteRenderer() override = default;
};