#pragma once

#include <iostream>
#include "../components/sprite_renderer.hpp"
#include "../components/ui_text_renderer.hpp"
#include "../components/ui_image_renderer.hpp"
#include "../system/window.hpp"

class RenderPipeline
{
private:
    std::weak_ptr<CameraComponent> _camera3d;
    std::weak_ptr<Transform> _cameraTransform3d;
    Camera _uiCamera;

    std::weak_ptr<Window> _window;

    std::vector<std::weak_ptr<SpriteRenderer>> _sprites;
    std::vector<std::weak_ptr<UiTextRenderer>> _texts;
    std::vector<std::weak_ptr<UiImageRenderer>> _images;

public:
    RenderPipeline(const std::weak_ptr<Window> &window)
    : _uiCamera(Camera(1, false, window)), _window(window)
    {
    }

    RenderPipeline(
        const std::weak_ptr<CameraComponent> &camera3d,
        const std::weak_ptr<Transform> &cameraTransform3d,
        const std::weak_ptr<Window> &window) :
                                               _camera3d(camera3d),
                                               _cameraTransform3d(cameraTransform3d),
                                               _uiCamera(Camera(1, false, window)) {
                                               };

    void UpdateCamera(const std::weak_ptr<CameraComponent> &camera3d,
                      const std::weak_ptr<Transform> &cameraTransform3d)
    {
        _camera3d = camera3d;
        _cameraTransform3d = cameraTransform3d;
    }

    void AddRenderer(const std::weak_ptr<SpriteRenderer> &sprite) { _sprites.push_back(sprite); }

    void AddRenderer(const std::weak_ptr<UiTextRenderer> &text) { _texts.push_back(text); }
    void AddRenderer(const std::weak_ptr<UiImageRenderer> &image) { _images.push_back(image); }

    void RenderUI() const;
    void RenderSprites() const;
    void Cleanup();
};