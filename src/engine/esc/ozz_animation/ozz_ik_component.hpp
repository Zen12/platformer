#pragma once
#include <string>
#include <glm/glm.hpp>

// IK chain type
enum class IKType {
    TwoBone,      // Two-bone IK (e.g., arm, leg)
    Aim,          // Aim constraint (look-at)
    // Future: FullBody, FABRIK, etc.
};

// Component for Inverse Kinematics
// Applies IK constraints on top of animation
class OzzIKComponent final {
public:
    IKType Type = IKType::TwoBone;
    bool Enabled = true;
    float Weight = 1.0f;  // Blend between animation and IK (0 = animation, 1 = full IK)

    // Bone indices (resolved from bone names at runtime)
    int StartBoneIndex = -1;   // Root of IK chain (e.g., shoulder, hip)
    int MidBoneIndex = -1;     // Middle joint (e.g., elbow, knee)
    int EndBoneIndex = -1;     // End effector (e.g., hand, foot)

    // Bone names (for serialization and lookup)
    std::string StartBoneName;
    std::string MidBoneName;
    std::string EndBoneName;

    // IK target (world space position)
    glm::vec3 Target = glm::vec3(0.0f);

    // Pole vector (hint direction for mid joint, e.g., knee direction)
    glm::vec3 PoleVector = glm::vec3(0.0f, 1.0f, 0.0f);

    // For Aim constraint
    glm::vec3 AimAxis = glm::vec3(0.0f, 0.0f, 1.0f);  // Forward axis
    glm::vec3 UpAxis = glm::vec3(0.0f, 1.0f, 0.0f);   // Up axis

    OzzIKComponent() = default;
};
