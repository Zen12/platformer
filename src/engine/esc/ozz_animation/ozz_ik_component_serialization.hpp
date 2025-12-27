#pragma once
#include "../../entity/component_serialization.hpp"
#include <string>
#include <glm/glm.hpp>

struct OzzIKComponentSerialization final : public ComponentSerialization {
    std::string Type = "two_bone";  // "two_bone" or "aim"
    bool Enabled = true;
    float Weight = 1.0f;

    std::string StartBoneName;
    std::string MidBoneName;
    std::string EndBoneName;

    glm::vec3 Target = glm::vec3(0.0f);
    glm::vec3 PoleVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 AimAxis = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 UpAxis = glm::vec3(0.0f, 1.0f, 0.0f);

    ~OzzIKComponentSerialization() override = default;
};
