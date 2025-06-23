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
    std::weak_ptr<Shader> _shader;
    std::weak_ptr<Material> _material;

public:
    SpriteRenderer() = delete;
    explicit SpriteRenderer(const std::weak_ptr<Entity> &entity)
    : Component(entity),
      _mesh(Mesh::GenerateSprite())
    {}

    void Update() const override;

    void SetSprite(std::weak_ptr<Sprite> sprite) noexcept;

    void SetMaterial(std::weak_ptr<Material> material) noexcept;

    void SetShader(std::weak_ptr<Shader> shader) noexcept;


    void Render() const noexcept;

    [[nodiscard]] int32_t GetShaderId() const noexcept
    {
        if (const auto shader = _shader.lock()) {
            return shader->GetShaderId();
        }
        return -1;
    }

    ~SpriteRenderer() override = default;
};