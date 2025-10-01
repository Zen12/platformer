#pragma once
#include <string>
#include <utility>

class CharacterControllerSettings {
public:
    float Damage{10};

    std::string AiTargetTransformTag{};

    CharacterControllerSettings() = default;

    CharacterControllerSettings(
        const float& damage,
        std::string aiTargetTag)
        :
            Damage(damage),
            AiTargetTransformTag(std::move(aiTargetTag))
    {}
};