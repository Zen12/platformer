#pragma once

class CharacterControllerSettings {
public:

    // movement
    float MaxMovementSpeed{5};
    float AccelerationSpeed{1};
    float Deceleration{1};

    // jump
    float JumpHeigh{1};
    float JumpDuration{1};
    float JumpDownMultiplier{1};

    CharacterControllerSettings() = default;

    CharacterControllerSettings(
        const float& movementSpeed,
        const float& accelerationSpeed,
        const float& deceleration,
        const float& jumpHeight,
        const float& jumpDuration,
        const float& jumpDownMultiplier)
        :
    MaxMovementSpeed(movementSpeed),
    AccelerationSpeed(accelerationSpeed),
    Deceleration(deceleration),
    JumpHeigh(jumpHeight),
    JumpDuration(jumpDuration),
    JumpDownMultiplier(jumpDownMultiplier)
    {}
};
