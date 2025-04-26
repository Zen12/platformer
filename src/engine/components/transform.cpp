#include "transform.h"


void Transform::Update() const 
{
}

glm::mat4 Transform::GetModel()
{
    glm::mat4 model = glm::mat4(1.0f); 

    if (auto entity = _entity.lock())
    {
        model = glm::scale(model, _scale);

        model = glm::rotate(model, glm::radians(_eulerRotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X
        model = glm::rotate(model, glm::radians(_eulerRotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y
        model = glm::rotate(model, glm::radians(_eulerRotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z

        model = glm::translate(model, _position);
    }

    return model;

}


void Transform::SetPosition(const glm::vec3 &position)
{
    _position = position;
}

glm::vec3 Transform::GetPosition() const
{
    return _position;
}


void Transform::SetEulerRotation(const glm::vec3 &euler)
{
    _eulerRotation = euler;
}

glm::vec3 Transform::GetEulerRotation() const
{
    return _eulerRotation;
}

void Transform::SetScale(const glm::vec3 &scale)
{
    _scale = scale;
}

glm::vec3 Transform::GetScale() const
{
    return _scale;
}
