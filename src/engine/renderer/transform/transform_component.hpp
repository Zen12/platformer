#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "../../entity/entity.hpp"
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

    void Update([[maybe_unused]] const float& deltaTime) noexcept override;

    [[nodiscard]] virtual glm::mat4 GetModel() const;

    void SetPosition(const glm::vec3 &position) noexcept;
    [[nodiscard]] glm::vec3 GetPosition() const noexcept;

    void SetPosition(const float& x, const float& y) noexcept;

    void SetEulerRotation(const glm::vec3 &euler) noexcept;
    [[nodiscard]] glm::vec3 GetEulerRotation() const noexcept;

    void SetScale(const glm::vec3 &scale) noexcept;
    [[nodiscard]] glm::vec3 GetScale() const noexcept;
};