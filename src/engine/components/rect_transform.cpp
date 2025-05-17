

#include "rect_transform.h"


glm::mat4 RectTransform::GetModel() const
{
    auto model = Transform::GetModel();
    model = glm::translate(model, glm::vec3(_rect.x, _rect.y, 0));
    model = glm::scale(model, glm::vec3(_rect.w, _rect.z, 1));
    return model;
}
