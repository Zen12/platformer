#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

class IKAimComponent final {
public:
    // Enable/disable IK
    bool Enabled = true;

    // Aim target in world space
    glm::vec3 AimTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    bool HasAimTarget = false;

    // Blend settings
    float BlendWeight = 1.0f;
    float BlendSpeed = 5.0f;
    float CurrentBlend = 0.0f;

    // Torso rotation limits (degrees)
    float TorsoMaxYaw = 60.0f;
    float TorsoMaxPitch = 30.0f;

    // Spine rotation distribution weights (should sum to 1.0)
    float SpineWeight = 0.3f;
    float Spine1Weight = 0.4f;
    float ChestWeight = 0.3f;

    // Hand offsets
    glm::vec3 RightHandOffset = glm::vec3(0.0f);
    glm::vec3 LeftHandOffset = glm::vec3(0.0f);
    glm::vec3 LeftHandGripPoint = glm::vec3(0.3f, 0.0f, 0.2f);

    // Elbow pole vectors (hint direction for elbow bend)
    glm::vec3 RightElbowHint = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 LeftElbowHint = glm::vec3(0.0f, 0.0f, -1.0f);

    // Recoil (0 = right hand, 1 = left hand)
    float RecoilTimer = 0.0f;
    float RecoilDuration = 0.3f;
    glm::vec3 RecoilOffsetRight = glm::vec3(0.0f, 0.3f, 0.0f);
    glm::vec3 RecoilOffsetLeft = glm::vec3(0.0f, 0.3f, 0.0f);
    int RecoilHand = 0;

    // Character rotation to face aim target
    bool RotateCharacterToAim = true;
    float CharacterRotationSpeed = 15.0f;

    // Movement direction relative to facing (for animation system)
    // 0 = forward, 180 = backward, 90 = right strafe, -90 = left strafe
    float MovementDirectionAngle = 0.0f;

    // Cached bone indices (built once from BoneNames)
    bool BoneIndicesCached = false;
    int HipsIndex = -1;
    int SpineIndex = -1;
    int Spine1Index = -1;
    int ChestIndex = -1;
    int NeckIndex = -1;
    int HeadIndex = -1;
    int ClavicleRIndex = -1;
    int UpperArmRIndex = -1;
    int LowerArmRIndex = -1;
    int HandRIndex = -1;
    int ClavicleLIndex = -1;
    int UpperArmLIndex = -1;
    int LowerArmLIndex = -1;
    int HandLIndex = -1;

    void CacheBoneIndices(const std::vector<std::string>& boneNames) {
        if (BoneIndicesCached) return;

        std::unordered_map<std::string, int> nameToIndex;
        for (size_t i = 0; i < boneNames.size(); ++i) {
            nameToIndex[boneNames[i]] = static_cast<int>(i);
        }

        auto findBone = [&nameToIndex](const std::string& name) -> int {
            auto it = nameToIndex.find(name);
            return (it != nameToIndex.end()) ? it->second : -1;
        };

        HipsIndex = findBone("hips");
        SpineIndex = findBone("spine");
        Spine1Index = findBone("spine1");
        ChestIndex = findBone("chest");
        NeckIndex = findBone("neck");
        HeadIndex = findBone("Head");
        ClavicleRIndex = findBone("clavicle_R");
        UpperArmRIndex = findBone("upper_arm_R");
        LowerArmRIndex = findBone("lower_arm_R");
        HandRIndex = findBone("hand_R");
        ClavicleLIndex = findBone("clavicle_L");
        UpperArmLIndex = findBone("upper_arm_L");
        LowerArmLIndex = findBone("lower_arm_L");
        HandLIndex = findBone("hand_L");

        BoneIndicesCached = true;
    }

    bool HasValidBoneIndices() const {
        return BoneIndicesCached &&
               SpineIndex >= 0 && Spine1Index >= 0 && ChestIndex >= 0 &&
               UpperArmRIndex >= 0 && LowerArmRIndex >= 0 && HandRIndex >= 0 &&
               UpperArmLIndex >= 0 && LowerArmLIndex >= 0 && HandLIndex >= 0;
    }
};
