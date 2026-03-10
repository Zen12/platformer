#pragma once
#include "node/action/action_serialiazion.hpp"

class StopVideoRecordingActionSerialization final : public ActionSerialization {
public:
    ~StopVideoRecordingActionSerialization() override = default;
};
