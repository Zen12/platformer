

#include "rect_transform.h"

const glm::vec4 RectTransform::GetAnchoredPosion() const
{
    return _rect; 
}

glm::mat4 RectTransform::GetModel() const
{
    return Transform::GetModel();
}
