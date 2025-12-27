#pragma once
#include "../../entity/component_serialization.hpp"
#include <string>

struct OzzAnimationComponentSerialization final : public ComponentSerialization {
    std::string AnimationGuid;
    float PlaybackSpeed = 1.0f;
    bool Loop = true;
    bool Playing = true;

    ~OzzAnimationComponentSerialization() override = default;
};
