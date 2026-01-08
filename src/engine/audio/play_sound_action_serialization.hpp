#pragma once
#include <string>
#include "../fsm/node/action/action_serialiazion.hpp"

class PlaySoundActionSerialization final : public ActionSerialization {
public:
    std::string AudioGuid;
    float Volume = 1.0f;
    bool Loop = false;

    ~PlaySoundActionSerialization() override = default;
};
