#pragma once
#include <string>
#include "action_serialiazion.hpp"

class StartVideoRecordingActionSerialization final : public ActionSerialization {
public:
    std::string OutputFile;
    int Fps = 60;

    ~StartVideoRecordingActionSerialization() override = default;
};
