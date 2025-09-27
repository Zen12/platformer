#include "rect_transform_component.hpp"

glm::mat4 RectTransform::GetModel() const {
    const auto model = Transform::GetModel();
    const auto parentModel = _parent.lock()->GetModel();

    const float scaleX = glm::length(glm::vec3(parentModel[0]));
    const float scaleY = glm::length(glm::vec3(parentModel[1]));

    return _constrains->Build(model, scaleX, scaleY);
}