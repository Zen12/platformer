#pragma once
#include <string>
#include <glm/glm.hpp>
#include "entity/component_serialization.hpp"

struct BoneAttachmentComponentSerialization final : public ComponentSerialization {
    std::string ParentTag;
    std::string BoneName;
    glm::vec3 PositionOffset = glm::vec3(0.0f);
    glm::vec3 RotationOffset = glm::vec3(0.0f);

    ~BoneAttachmentComponentSerialization() override = default;
};
