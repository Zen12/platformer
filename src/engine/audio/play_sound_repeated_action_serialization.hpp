#pragma once
#include "../fsm/node/action/action_serialiazion.hpp"
#include "../system/guid.hpp"

class PlaySoundRepeatedActionSerialization final : public ActionSerialization {
public:
    Guid AudioGuid;
    float Volume = 1.0f;
    float DelaySeconds = 0.5f;
    bool Spatial = true;
    float MinDistance = 1.0f;
    float MaxDistance = 50.0f;

    ~PlaySoundRepeatedActionSerialization() override = default;
};
