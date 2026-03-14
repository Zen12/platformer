#pragma once
#include <string>
#include "node/action/action_serialization.hpp"

class StartVideoRecordingActionSerialization final : public ActionSerialization {
public:
    std::string OutputFile;
    int Fps = 60;

    ~StartVideoRecordingActionSerialization() override = default;
};
