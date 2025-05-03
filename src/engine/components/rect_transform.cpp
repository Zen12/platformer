

#include "rect_transform.h"

const glm::vec3 RectTransform::GetAnchoredPosion(const uint16_t &canvasWidth, const uint16_t &canvasHeight) const
{
    return _rect;
}

glm::mat4 RectTransform::GetModel() const
{
    return Transform::GetModel(); // * _rect
}
