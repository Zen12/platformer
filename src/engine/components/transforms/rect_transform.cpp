

#include "rect_transform.h"


glm::mat4 RectTransform::GetModel() const
{
    const auto model = Transform::GetModel();
    const auto parentModel = _parent.lock()->GetModel();

    float scaleX = glm::length(glm::vec3(parentModel[0]));
    float scaleY = glm::length(glm::vec3(parentModel[1]));

    return _constrains->Build(model, scaleX, scaleY);
}
