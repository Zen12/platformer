#pragma once

#include "../render/font.h"
#include "../render/mesh.h"
#include "../render/shader.h"
#include "../asset/asset_loader.h"
#include "../components/transform.h"
#include "../components/rect_transform.h"
#include "entity.h"

class UiImageRenderer : public Component
{
    private:
        std::weak_ptr<Sprite> _sprite;
        Mesh _mesh;
        Shader _shader;

    public:
        UiImageRenderer(const std::weak_ptr<Entity>& entity): Component(entity),
            _mesh(Mesh::GenerateSprite()),
            _shader(AssetLoader::LoadShaderFromPath("shaders/uiImage_vert.glsl", "shaders/uiImage_frag.glsl"))
        {}

        void Update() const override;
        void SetSprite(std::weak_ptr<Sprite> sprite);
        void Render(const uint16_t& width, const uint16_t& height) const;

        uint32_t GetShaderId() const
        {
            return _shader.GetShaderId();
        }

    private:
        void Bind() ;
};