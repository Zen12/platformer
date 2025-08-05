#pragma once

class CharacterControllerSettings {
public:

    float Speed{1};
    float JumpPower{1};

    CharacterControllerSettings(const float& speed, const float& jumpPower)
        : Speed(speed), JumpPower(jumpPower) {}

};
