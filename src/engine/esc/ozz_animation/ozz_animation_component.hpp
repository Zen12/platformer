#pragma once
#include <string>

// Component for ozz-animation playback
// References an ozz animation asset and controls playback
class OzzAnimationComponent final {
public:
    std::string AnimationGuid;  // GUID of the ozz animation asset
    float Time = 0.0f;          // Current playback time in seconds
    float PlaybackSpeed = 1.0f; // Speed multiplier
    bool Loop = true;           // Loop animation
    bool Playing = true;        // Is animation playing
    float Weight = 1.0f;        // Blend weight (for future blending support)

    OzzAnimationComponent() = default;
    explicit OzzAnimationComponent(const std::string& animGuid)
        : AnimationGuid(animGuid) {}
};
