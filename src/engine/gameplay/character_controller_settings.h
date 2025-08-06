#pragma once

class CharacterControllerSettings {
public:

    // movement
    float MovementSpeed{1};

    // jump
    float JumpHeigh{1};
    float JumpDuration{1};
    float JumpDownMultiplier{1};

    CharacterControllerSettings() = default;

    CharacterControllerSettings(const float& movementSpeed, const float& jumpHeight, const float& jumpDuration, const float& jumpDownMultiplier)
        : MovementSpeed(movementSpeed), JumpHeigh(jumpHeight), JumpDuration(jumpDuration), JumpDownMultiplier(jumpDownMultiplier)
    {}
};
