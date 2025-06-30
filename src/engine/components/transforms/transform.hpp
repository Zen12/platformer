#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "../entity.hpp"
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transform : public Component
{

protected:
    glm::vec3 _position = glm::vec3(0);
    glm::vec3 _eulerRotation = glm::vec3(0);
    glm::vec3 _scale = glm::vec3(1);

public:
    Transform() = delete;
    explicit Transform(const std::weak_ptr<Entity> &entity) : Component(entity)
    {
    }
    ~Transform() override = default;

    void Update() const override;

    [[nodiscard]] virtual glm::mat4 GetModel() const;

    void SetPosition(const glm::vec3 &position);
    [[nodiscard]] glm::vec3 GetPosition() const;

    void SetPosition(const float& x, const float& y);

    void SetEulerRotation(const glm::vec3 &euler);
    [[nodiscard]] glm::vec3 GetEulerRotation() const;

    void SetScale(const glm::vec3 &scale);
    [[nodiscard]] glm::vec3 GetScale() const;
};