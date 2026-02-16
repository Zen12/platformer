#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Keyframe for position
struct PositionKey {
    float Time;
    glm::vec3 Value;
};

// Keyframe for rotation
struct RotationKey {
    float Time;
    glm::quat Value;
};

// Keyframe for scale
struct ScaleKey {
    float Time;
    glm::vec3 Value;
};

// Animation channel for a single bone
struct BoneAnimationChannel {
    std::string BoneName;
    std::vector<PositionKey> PositionKeys;
    std::vector<RotationKey> RotationKeys;
    std::vector<ScaleKey> ScaleKeys;
};

// Complete animation data
struct AnimationData {
    std::string Name;
    float Duration;           // Duration in seconds
    float TicksPerSecond;     // Framerate
    std::vector<BoneAnimationChannel> Channels;
};
