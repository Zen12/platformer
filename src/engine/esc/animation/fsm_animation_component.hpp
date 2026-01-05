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

    // Animation speed settings
    float AnimationSpeed = 1.0f;  // Base animation speed multiplier
    bool VelocityBasedSpeed = false;  // If true, animation speed scales with agent velocity
    float VelocitySpeedScale = 1.0f;  // How much velocity affects animation speed (velocity / MaxSpeed * scale)

    // Per-state overrides (set by animation_state action)
    float StateAnimationSpeed = -1.0f;  // Override speed for current state (-1 = use default)
    bool StateDisableVelocitySpeed = false;  // Disable velocity-based speed for current state
    bool MovementDisabled = false;  // Disable player movement (checked by PlayerControllerSystem)
    float MovementDisabledDuration = -1.0f;  // How long to disable movement (-1 = until state exits)
    float MovementDisabledTimer = 0.0f;  // Current timer for movement disable

    // Trigger to set when animation completes
    std::string OnCompleteTrigger;
    bool HasCompletedOnce = false;  // Track if non-looping animation has completed

    // State tracking for trigger detection
    bool WasJumping = false;  // Track previous jump state for edge detection

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
