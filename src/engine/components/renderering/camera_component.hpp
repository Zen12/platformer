#pragma once

#include "../../render/camera.hpp"
#include "../entity.hpp"
#include <glm/glm.hpp>

class CameraComponent final : public Component
{
private:
    Camera _camera{};
    std::weak_ptr<Window> _window;
    std::weak_ptr<Transform> _transform;

public:
    explicit CameraComponent(const std::weak_ptr<Entity> &entity) : Component(entity)
    {
        _transform = entity.lock()->GetComponent<Transform>();
    }

    void SetCamera(const Camera &camera)
    {
        _camera = camera;
    }

    void SetWindow(const std::weak_ptr<Window> &window) noexcept {
        _window = window;
    }

    [[nodiscard]] glm::mat4 GetProjection() const {
        return _camera.GetProjection();
    }

    void Update([[maybe_unused]] const float& deltaTime) override
    {
    }

    [[nodiscard]] glm::vec3 ScreenToWorldPoint( glm::vec3 screenPos) const {

        if (const auto window = _window.lock()) {

            if (const auto tr = _transform.lock()) {

                const glm::vec4 viewport(0.0f, 0.0f, window->GetWidth(), window->GetHeight());

                const auto view = tr->GetModel();
                // Unproject
                return glm::unProject(screenPos, view, GetProjection(), viewport);
            }
        }

        return glm::vec3(0.0f);
    }

    [[nodiscard]] glm::vec3 WorldToScreenPoint(glm::vec3 worldPos) const {
        if (const auto window = _window.lock()) {
            if (const auto tr = _transform.lock()) {

                const glm::vec4 viewport(0.0f, 0.0f, window->GetWidth(), window->GetHeight());

                const auto view = tr->GetModel(); // careful: if this is actually "model" and not "view", see note below

                // Project world -> screen
                const glm::vec3 screenPos = glm::project(worldPos, view, GetProjection(), viewport);

                return screenPos;
            }
        }
        return glm::vec3(0.0f);
    }


    [[nodiscard]] glm::vec3 ScreenToWorldPoint( glm::vec2 screenPos) const {
        return ScreenToWorldPoint(glm::vec3(screenPos.x, screenPos.y, 0));
    }
};