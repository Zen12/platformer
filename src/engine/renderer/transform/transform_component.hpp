#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "../../entity/entity.hpp"
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "transform_component_serialization.hpp"


class TransformComponent final {
private:
    glm::vec3 _position = glm::vec3(0);
    glm::vec3 _eulerRotation = glm::vec3(0);
    glm::vec3 _scale = glm::vec3(1);
public:
    TransformComponent() = default;
    explicit TransformComponent(const TransformComponentSerialization& serialization) :
        _position(serialization.position), _eulerRotation(serialization.rotation), _scale(serialization.scale)
    {}

    [[nodiscard]] glm::mat4 GetModel() const {
        auto model = glm::scale(glm::mat4(1.0f), _scale);

        model = glm::rotate(model, glm::radians(_eulerRotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X
        model = glm::rotate(model, glm::radians(_eulerRotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y
        model = glm::rotate(model, glm::radians(_eulerRotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z

        model = glm::translate(model, _position);

        return model;
    }
};

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