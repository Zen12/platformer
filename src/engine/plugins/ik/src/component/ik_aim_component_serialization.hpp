#pragma once
#include "entity/component_serialization.hpp"
#include <glm/glm.hpp>

struct IKAimComponentSerialization final : public ComponentSerialization {
    bool Enabled = true;

    float BlendWeight = 1.0f;
    float BlendSpeed = 5.0f;

    float TorsoMaxYaw = 60.0f;
    float TorsoMaxPitch = 30.0f;

    float SpineWeight = 0.3f;
    float Spine1Weight = 0.4f;
    float ChestWeight = 0.3f;

    glm::vec3 RightHandOffset = glm::vec3(0.0f);
    glm::vec3 LeftHandOffset = glm::vec3(0.0f);
    glm::vec3 LeftHandGripPoint = glm::vec3(0.3f, 0.0f, 0.2f);

    glm::vec3 RightElbowHint = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 LeftElbowHint = glm::vec3(0.0f, 0.0f, -1.0f);

    bool RotateCharacterToAim = true;
    float CharacterRotationSpeed = 15.0f;

    float RecoilDuration = 0.3f;
    glm::vec3 RecoilOffsetRight = glm::vec3(0.0f, 0.3f, 0.0f);
    glm::vec3 RecoilOffsetLeft = glm::vec3(0.0f, 0.3f, 0.0f);
};
