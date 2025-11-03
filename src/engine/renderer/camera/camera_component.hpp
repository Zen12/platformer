#pragma once

#include "camera.hpp"
#include "../../entity/entity.hpp"
#include "../transform/transform_component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#include "camera_component_serialization.hpp"

class CameraComponentComponent final {
private:
    CameraComponentSerialization _cameraSerialization{};
    Camera _camera{};

public:

    CameraComponentComponent() = default;

    explicit CameraComponentComponent(CameraComponentSerialization cameraSerialization, const std::weak_ptr<Window>& window)
        : _cameraSerialization(std::move(cameraSerialization)),
        _camera(_cameraSerialization.aspectPower, false, _cameraSerialization.isPerspective, window)
    {}

    [[nodiscard]] glm::mat4 GetProjection() const { return _camera.GetProjection(); }

    [[nodiscard]] glm::vec3 ScreenToWorldPoint([[maybe_unused]] glm::vec3 screenPos) const
    {
        // todo
        return glm::vec3(0.0f);
    }

    [[nodiscard]] glm::vec3 ScreenToWorldPoint(const glm::vec2 &screenPos) const
    {
        return ScreenToWorldPoint(glm::vec3(screenPos.x, screenPos.y, 0));
    }

    [[nodiscard]] glm::vec3 WorldToScreenPoint([[maybe_unused]] glm::vec3 worldPos) const
    {
        // todo
        return glm::vec3(0.0f);
    }
};

class CameraComponent final : public Component
{
private:
    Camera _camera{};
    std::weak_ptr<Window> _window{};
    std::weak_ptr<Transform> _transform{};

public:
    explicit CameraComponent(const std::weak_ptr<Entity> &entity);

    void SetCamera(const Camera &camera) { _camera = camera; }
    void SetWindow(const std::weak_ptr<Window> &window) { _window = window; }

    [[nodiscard]] glm::mat4 GetProjection() const { return _camera.GetProjection(); }
    [[nodiscard]] glm::vec3 ScreenToWorldPoint(glm::vec3 screenPos) const;
    [[nodiscard]] glm::vec3 ScreenToWorldPoint(glm::vec2 screenPos) const;
    [[nodiscard]] glm::vec3 WorldToScreenPoint(glm::vec3 worldPos) const;

    void Update([[maybe_unused]] const float& deltaTime) override;
};