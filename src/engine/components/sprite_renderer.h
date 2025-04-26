#pragma once

#include "entity.h"
#include "../asset/asset_loader.h"
#include "../render/mesh.h"
#include "transform.h"
#include "camera_component.h"


class SpriteRenderer : public Component {

    private:
        std::weak_ptr<Sprite> _sprite;
        std::weak_ptr<CameraComponent> _cameraComponent;
        Mesh _mesh;
        Shader _shader;


    public:
        SpriteRenderer() = delete;
        SpriteRenderer(const std::weak_ptr<Entity>& entity) : Component(entity), 

            _mesh(Mesh::GenerateSprite()),
            _shader(AssetLoader::LoadShaderFromPath("shaders/sprite_vert.glsl", "shaders/sprite_frag.glsl"))
        {
        };

        void SetSprite(std::weak_ptr<Sprite> sprite);
        void SetCamera(std::weak_ptr<CameraComponent> camera);

        void Update() const override;

        uint32_t GetShaderId() const
        {
            return _shader.GetShaderId();
        }

        ~SpriteRenderer() override = default;

};