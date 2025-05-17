

#include "rect_transform.h"

const glm::vec4 RectTransform::GetAnchoredPosion() const
{
    return _rect; 
}

const std::array<glm::vec2, 4> RectTransform::GetCorners() const {
    return std::array<glm::vec2, 4> {
            glm::vec2(_rect.x, _rect.y),
            glm::vec2(_rect.y, _rect.y),
            glm::vec2(_rect.x + _rect.z, _rect.y),
            glm::vec2(_rect.x + _rect.z, _rect.y + _rect.w)

    };
}

glm::mat4 RectTransform::GetModel() const
{
    return Transform::GetModel();
}
