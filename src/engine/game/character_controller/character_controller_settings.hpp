#pragma once
#include <string>

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

    float AirControl{0.1};

    float Damage{10};

    std::string AiTargetTransformTag{};

    CharacterControllerSettings() = default;

    CharacterControllerSettings(
        const float& movementSpeed,
        const float& accelerationSpeed,
        const float& deceleration,
        const float& jumpHeight,
        const float& jumpDuration,
        const float& jumpDownMultiplier,
        const float& airControl,
        const float& damage,
        const std::string&aiTargetTag)
        :
            MaxMovementSpeed(movementSpeed),
            AccelerationSpeed(accelerationSpeed),
            Deceleration(deceleration),
            JumpHeigh(jumpHeight),
            JumpDuration(jumpDuration),
            JumpDownMultiplier(jumpDownMultiplier),
            AirControl(airControl),
            Damage(damage),
            AiTargetTransformTag(aiTargetTag)
    {}
};