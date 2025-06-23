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
    Shader _shader;

public:
    SpriteRenderer() = delete;
    explicit SpriteRenderer(const std::weak_ptr<Entity> &entity)
    : Component(entity),
      _mesh(Mesh::GenerateSprite()),
       _shader(AssetLoader::LoadShaderFromPath(ASSETS_PATH"resources/shaders/sprite.vert",ASSETS_PATH "resources/shaders/sprite.frag"))
    {}

    void Update() const override;

    void SetSprite(std::weak_ptr<Sprite> sprite) noexcept;

    void SetMaterial(std::weak_ptr<Material> material) noexcept;

    void Render() const noexcept;

    [[nodiscard]] int32_t GetShaderId() const noexcept
    {
        return _shader.GetShaderId();
    }

    ~SpriteRenderer() override = default;
};