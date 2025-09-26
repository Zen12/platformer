#include "camera_component.hpp"

CameraComponent::CameraComponent(const std::weak_ptr<Entity> &entity)
    : Component(entity)
{
    _transform = entity.lock()->GetComponent<Transform>();
}

void CameraComponent::Update([[maybe_unused]] const float& deltaTime)
{
}

glm::vec3 CameraComponent::ScreenToWorldPoint(glm::vec3 screenPos) const
{
    if (const auto window = _window.lock()) {
        if (const auto tr = _transform.lock()) {
            const glm::vec4 viewport(0.0f, 0.0f, window->GetWidth(), window->GetHeight());
            const auto view = tr->GetModel();
            return glm::unProject(screenPos, view, GetProjection(), viewport);
        }
    }
    return glm::vec3(0.0f);
}

glm::vec3 CameraComponent::ScreenToWorldPoint(glm::vec2 screenPos) const
{
    return ScreenToWorldPoint(glm::vec3(screenPos.x, screenPos.y, 0));
}

glm::vec3 CameraComponent::WorldToScreenPoint(glm::vec3 worldPos) const
{
    if (const auto window = _window.lock()) {
        if (const auto tr = _transform.lock()) {
            const glm::vec4 viewport(0.0f, 0.0f, window->GetWidth(), window->GetHeight());
            const auto view = tr->GetModel();
            const glm::vec3 screenPos = glm::project(worldPos, view, GetProjection(), viewport);
            return screenPos;
        }
    }
    return glm::vec3(0.0f);
}