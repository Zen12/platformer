#pragma once
#include "../../../entity/component_serialization.hpp"

struct CharacterMovementComponentSerialization final : public ComponentSerialization
{
    // movement
    float MaxMovementSpeed{1};
    float AccelerationSpeed{1};
    float DecelerationSpeed{1};

    // jump
    float JumpHeight{1};
    float JumpDuration{1};
    float JumpDownMultiplier{1};
    float AirControl{0.1};

    CharacterMovementComponentSerialization() = default;

    CharacterMovementComponentSerialization(
        const float& movementSpeed,
        const float& accelerationSpeed,
        const float& deceleration,
        const float& jumpHeight,
        const float& jumpDuration,
        const float& jumpDownMultiplier,
        const float& airControl)
        :
            MaxMovementSpeed(movementSpeed),
            AccelerationSpeed(accelerationSpeed),
            DecelerationSpeed(deceleration),
            JumpHeight(jumpHeight),
            JumpDuration(jumpDuration),
            JumpDownMultiplier(jumpDownMultiplier),
            AirControl(airControl)
    {}
};