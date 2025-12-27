#pragma once
#include "../../entity/component_serialization.hpp"
#include <glm/glm.hpp>

struct MouseIKControllerComponentSerialization final : public ComponentSerialization {
    bool Enabled = true;
    glm::vec3 PlaneNormal = glm::vec3(0.0f, 1.0f, 0.0f);
    float PlaneDistance = 0.0f;
    glm::vec3 TargetOffset = glm::vec3(0.0f, 0.0f, 0.0f);

    ~MouseIKControllerComponentSerialization() override = default;
};
