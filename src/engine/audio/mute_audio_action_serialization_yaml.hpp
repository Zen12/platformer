#pragma once
#include "mute_audio_action_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<MuteAudioActionSerialization> {
        static bool decode(const Node& node, MuteAudioActionSerialization& rhs) {
            rhs.Mute = node["mute"] ? node["mute"].as<bool>() : true;
            rhs.OnlyDebug = node["only_debug"] ? node["only_debug"].as<bool>() : false;
            return true;
        }
    };
}
