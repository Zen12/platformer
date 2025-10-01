#pragma once

struct CharacterMovementSettings {
    // movement
    float MaxMovementSpeed{5};
    float AccelerationSpeed{1};
    float Deceleration{1};

    // jump
    float JumpHeigh{1};
    float JumpDuration{1};
    float JumpDownMultiplier{1};

    float AirControl{0.1};
};