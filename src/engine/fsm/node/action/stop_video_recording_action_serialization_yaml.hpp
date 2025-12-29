#pragma once
#include "stop_video_recording_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<StopVideoRecordingActionSerialization> {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] StopVideoRecordingActionSerialization &rhs) {
            return true;
        }
    };
}
