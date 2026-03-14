#pragma once
#include "node/action/action_serialization.hpp"

class MuteAudioActionSerialization final : public ActionSerialization {
public:
    bool Mute = true;
    bool OnlyDebug = false;

    ~MuteAudioActionSerialization() override = default;
};
