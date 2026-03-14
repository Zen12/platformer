#pragma once
#include "node/action/action_serialization.hpp"

class StopVideoRecordingActionSerialization final : public ActionSerialization {
public:
    ~StopVideoRecordingActionSerialization() override = default;
};
