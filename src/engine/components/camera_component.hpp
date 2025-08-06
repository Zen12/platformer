#pragma once

#include "../render/camera.hpp"
#include "entity.hpp"
#include <glm/glm.hpp>

class CameraComponent final : public Component
{
private:
    Camera _camera{};

public:
    explicit CameraComponent(const std::weak_ptr<Entity> &entity) : Component(entity)
    {
    }

    void SetCamera(const Camera &camera)
    {
        _camera = camera;
    }

    [[nodiscard]] glm::mat4 GetProjection() const {
        return _camera.GetProjection();
    }

    void Update([[maybe_unused]] const float& deltaTime) override
    {
    }
};