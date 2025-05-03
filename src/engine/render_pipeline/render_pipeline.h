#pragma once

#include <iostream>
#include "../components/sprite_renderer.h"
#include "../components/ui_text_renderer.h"
#include "../system/window.h"


class RenderPipeline
{
    private:
        Shader _uiShader;
        Camera _uiCamera;
        
        std::weak_ptr<Transform> _cameraTransform3d;
        std::weak_ptr<CameraComponent> _camera3d;

        std::weak_ptr<Window> _window;

        std::vector<std::weak_ptr<SpriteRenderer>> _sprites;
        std::vector<std::weak_ptr<UiTextRenderer>> _texts;
    
    public:

        RenderPipeline(
            const std::weak_ptr<CameraComponent>& camera3d, 
            const std::weak_ptr<Transform>& cameraTransform3d,
            const std::weak_ptr<Window>& window) : 
            
            _uiShader(AssetLoader::LoadShaderFromPath("shaders/text_vert.glsl","shaders/text_frag.glsl")),
            _uiCamera(Camera::Ortho(window)),
            _window(window),
            _camera3d(camera3d),
            _cameraTransform3d(cameraTransform3d)
        {
        };

        void AddRenderer(const std::weak_ptr<SpriteRenderer>& sprite) {_sprites.push_back(sprite);}

        void AddRenderer(const std::weak_ptr<UiTextRenderer>& text) {_texts.push_back(text);}

        void RenderUI() const;
        void RenderSprites() const;
        void Cleanup();
};