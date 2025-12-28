#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

// Animation component that plays loaded animation data
class SimpleAnimationComponent final {
public:
    std::string AnimationGuid;  // GUID of the animation asset
    float Time = 0.0f;
    bool Enabled = true;
    bool Loop = true;

    bool ApplyRootMotion = false;
    std::string RootBoneName = "mixamorig:Hips_01";
    glm::vec3 PreviousRootPosition = glm::vec3(0.0f);
    glm::quat PreviousRootRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    bool FirstFrame = true;

    SimpleAnimationComponent() = default;
    explicit SimpleAnimationComponent(const std::string& animGuid)
        : AnimationGuid(animGuid) {}
};
