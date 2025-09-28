#pragma once

#include "camera.hpp"
#include "../../components/entity.hpp"
#include "../transform/transform_component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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