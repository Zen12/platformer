#pragma once

#include "../render/camera.hpp"
#include "entity.hpp"
#include <glm/glm.hpp>

class CameraComponent : public Component
{
private:
    Camera _camera;

public:
    CameraComponent(const std::weak_ptr<Entity> &entity) : Component(entity)
    {
    }

    void SetCamera(Camera camera)
    {
        _camera = camera;
    }

    glm::mat4 GetProjection()
    {
        return _camera.GetProjection();
    }

    void Update() const override
    {
    }
};