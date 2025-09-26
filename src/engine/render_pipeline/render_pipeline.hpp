#pragma once

#include "../components/renderering/sprite_renderer.hpp"
#include "../ui/text/text_renderer_component.hpp"
#include "../ui/image/image_renderer_component.hpp"
#include "../system/window.hpp"
#include "../camera/camera_component.hpp"
#include "../components/renderering/particle_emitter.hpp"
#include "../components/renderering/spine_renderer.hpp"


class RenderPipeline
{
private:
    std::weak_ptr<CameraComponent> _camera3d;
    std::weak_ptr<Transform> _cameraTransform3d;
    Camera _uiCamera;

    std::weak_ptr<Window> _window;

    std::vector<std::weak_ptr<SpriteRenderer>> _sprites;
    std::vector<std::weak_ptr<MeshRenderer>> _meshRenderers;
    std::vector<std::weak_ptr<UiTextRendererComponent>> _texts;
    std::vector<std::weak_ptr<UiImageRendererComponent>> _images;
    std::vector<std::weak_ptr<ParticleEmitterComponent>> _particles;

public:
    explicit RenderPipeline(const std::weak_ptr<Window> &window)
    : _uiCamera(Camera(1, true, false, window)), _window(window)
    {
    }

    RenderPipeline(
        const std::weak_ptr<CameraComponent> &camera3d,
        const std::weak_ptr<Transform> &cameraTransform3d,
        const std::weak_ptr<Window> &window) :
                                               _camera3d(camera3d),
                                               _cameraTransform3d(cameraTransform3d),
                                               _uiCamera(Camera(1, true, false, window)) {
                                               };

    void UpdateCamera(const std::weak_ptr<CameraComponent> &camera3d,
                      const std::weak_ptr<Transform> &cameraTransform3d)
    {
        _camera3d = camera3d;
        _cameraTransform3d = cameraTransform3d;
    }

    void AddRenderer(const std::weak_ptr<MeshRenderer> &meshRenderer) noexcept { _meshRenderers.push_back(meshRenderer); }
    void AddRenderer(const std::weak_ptr<SpriteRenderer> &sprite) noexcept { _sprites.push_back(sprite); }

    void AddRenderer(const std::weak_ptr<UiTextRendererComponent> &text) noexcept { _texts.push_back(text); }
    void AddRenderer(const std::weak_ptr<UiImageRendererComponent> &image) noexcept { _images.push_back(image); }

    void AddRenderer(const std::weak_ptr<ParticleEmitterComponent> &particle) noexcept {
        _particles.push_back(particle);
    }

    void Init() const noexcept;
    void ClearFrame() const noexcept;
    void RenderUI(const float& deltaTime) const;
    void RenderSprites(const float& deltaTime) const;
    void RenderMeshes(const float& deltaTime);
    void RenderParticles(const float& deltaTime) const;

    [[nodiscard]] glm::vec3 ScreenToWorldPoint( glm::vec2 screenPos) const;

    [[nodiscard]] glm::vec3 ScreenToWorldPoint( const glm::vec3 &screenPos) const;

    [[nodiscard]] std::weak_ptr<CameraComponent> Get3dCamera() const { return _camera3d; }
    void RenderDebugLines() const;

    void Cleanup();
};
