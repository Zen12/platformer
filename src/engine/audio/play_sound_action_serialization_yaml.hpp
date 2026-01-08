#pragma once
#include "play_sound_action_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<PlaySoundActionSerialization> {
        static bool decode(const Node& node, PlaySoundActionSerialization& rhs) {
            rhs.AudioGuid = node["audio_guid"] ? node["audio_guid"].as<std::string>() : "";
            rhs.Volume = node["volume"] ? node["volume"].as<float>() : 1.0f;
            rhs.Loop = node["loop"] ? node["loop"].as<bool>() : false;
            return true;
        }
    };
}
