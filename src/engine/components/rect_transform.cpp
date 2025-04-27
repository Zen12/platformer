

#include "rect_transform.h"

const glm::vec4 RectTransform::GetRect() const
{
    return _rect;
}

glm::mat4 RectTransform::GetModel() const
{
    return Transform::GetModel(); // * _rect
}
