#pragma once
#include "start_video_recording_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<StartVideoRecordingActionSerialization> {
        static bool decode(const Node &node, StartVideoRecordingActionSerialization &rhs) {
            if (node["output_file"]) {
                rhs.OutputFile = node["output_file"].as<std::string>();
            }
            if (node["fps"]) {
                rhs.Fps = node["fps"].as<int>();
            }
            return true;
        }
    };
}
