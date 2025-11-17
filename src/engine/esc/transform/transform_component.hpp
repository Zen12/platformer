#pragma once
#include "glm/vec3.hpp"

#include "transform_component_serialization.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtx/transform.hpp"


class TransformComponentV2 final {
private:
    glm::vec3 _position = glm::vec3(0);
    glm::vec3 _eulerRotation = glm::vec3(0);
    glm::vec3 _scale = glm::vec3(1);
public:
    explicit TransformComponentV2(const TransformComponentSerialization& serialization) :
        _position(serialization.position), _eulerRotation(serialization.rotation), _scale(serialization.scale)
    {}

    [[nodiscard]] glm::mat4 GetModel() const {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), _position);

        const glm::mat4 RX = glm::rotate(glm::mat4(1.0f), glm::radians(_eulerRotation.x), glm::vec3(1,0,0));
        const glm::mat4 RY = glm::rotate(glm::mat4(1.0f), glm::radians(_eulerRotation.y), glm::vec3(0,1,0));
        const glm::mat4 RZ = glm::rotate(glm::mat4(1.0f), glm::radians(_eulerRotation.z), glm::vec3(0,0,1));

        const glm::mat4 S = glm::scale(glm::mat4(1.0f), _scale);

        return T * RZ * RY * RX * S;
    }

    [[nodiscard]] const glm::vec3& GetPosition() const noexcept {
        return _position;
    }
};
