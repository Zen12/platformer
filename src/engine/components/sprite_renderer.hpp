#pragma once

#include "entity.hpp"
#include "../asset/asset_loader.hpp"
#include "../render/mesh.hpp"

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
       _shader(AssetLoader::LoadShaderFromPath("shaders/sprite_vert.glsl", "shaders/sprite_frag.glsl"))
    {}

    void Update() const override;

    void SetSprite(std::weak_ptr<Sprite> sprite);

    void Render() const;

    [[nodiscard]] int32_t GetShaderId() const
    {
        return _shader.GetShaderId();
    }

    ~SpriteRenderer() override = default;
};