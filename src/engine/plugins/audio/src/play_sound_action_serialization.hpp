#pragma once
#include "node/action/action_serialization.hpp"
#include "guid.hpp"

class PlaySoundActionSerialization final : public ActionSerialization {
public:
    Guid AudioGuid;
    float Volume = 1.0f;
    bool Loop = false;

    ~PlaySoundActionSerialization() override = default;
};
