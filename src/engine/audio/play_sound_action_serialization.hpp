#pragma once
#include "../fsm/node/action/action_serialiazion.hpp"
#include "../system/guid.hpp"

class PlaySoundActionSerialization final : public ActionSerialization {
public:
    Guid AudioGuid;
    float Volume = 1.0f;
    bool Loop = false;

    ~PlaySoundActionSerialization() override = default;
};
