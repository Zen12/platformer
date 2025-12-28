#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utility>

// Forward declaration
class FsmController;

class FsmAnimationComponent final {
public:
    std::string FsmGuid;  // GUID of the FSM that controls animation
    std::string CurrentAnimationGuid;  // Set by FSM action
    float Time = 0.0f;
    bool Loop = true;

    // Trigger to set when animation completes
    std::string OnCompleteTrigger;
    bool HasCompletedOnce = false;  // Track if non-looping animation has completed

    // Transition state
    bool IsTransitioning = false;
    std::string FromAnimationGuid;
    std::string ToAnimationGuid;
    float TransitionDuration = 0.0f;
    float TransitionProgress = 0.0f;

    // FSM Controller that manages animation states
    std::shared_ptr<FsmController> Controller;

    // Shared pointer to this component (used by FsmController actions)
    // Must be kept alive for weak_ptr references in actions to work
    std::shared_ptr<FsmAnimationComponent> SelfPtr;

    FsmAnimationComponent() = default;
    explicit FsmAnimationComponent(std::string  fsmGuid)
        : FsmGuid(std::move(fsmGuid)) {}
};
