#pragma once
#include "audio_source_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<AudioSourceComponentSerialization> {
        static bool decode(const Node& node, AudioSourceComponentSerialization& rhs) {
            rhs.AudioClipGuid = node["audio_clip_guid"] ? node["audio_clip_guid"].as<std::string>() : "";
            rhs.Volume = node["volume"] ? node["volume"].as<float>() : 1.0f;
            rhs.Pitch = node["pitch"] ? node["pitch"].as<float>() : 1.0f;
            rhs.Loop = node["loop"] ? node["loop"].as<bool>() : false;
            rhs.AutoPlay = node["auto_play"] ? node["auto_play"].as<bool>() : false;
            rhs.Spatial = node["spatial"] ? node["spatial"].as<bool>() : true;
            rhs.MinDistance = node["min_distance"] ? node["min_distance"].as<float>() : 1.0f;
            rhs.MaxDistance = node["max_distance"] ? node["max_distance"].as<float>() : 100.0f;
            return true;
        }
    };
}
