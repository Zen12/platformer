#pragma once
#include <string>
#include <glm/glm.hpp>
#include "../../entity/component_serialization.hpp"

struct TopDownCameraComponentSerialization final : public ComponentSerialization {
    std::string TargetTag;
    glm::vec3 OffsetPosition{0.0f, 10.0f, 10.0f};
    glm::vec3 OffsetRotation{-45.0f, 0.0f, 0.0f};
    float MaxLookAhead{5.0f};
    float SmoothSpeed{5.0f};
    float MaxMoveSpeed{15.0f};
};
