#pragma once
#include "play_sound_repeated_action_serialization.hpp"
#include "yaml-cpp/node/node.h"
#include "guid_yaml.hpp"

namespace YAML {
    template <>
    struct convert<PlaySoundRepeatedActionSerialization> {
        static bool decode(const Node &node, PlaySoundRepeatedActionSerialization &rhs) {
            rhs.AudioGuid = node["audio_guid"].as<Guid>();
            if (node["volume"]) {
                rhs.Volume = node["volume"].as<float>();
            }
            if (node["delay_seconds"]) {
                rhs.DelaySeconds = node["delay_seconds"].as<float>();
            }
            if (node["spatial"]) {
                rhs.Spatial = node["spatial"].as<bool>();
            }
            if (node["min_distance"]) {
                rhs.MinDistance = node["min_distance"].as<float>();
            }
            if (node["max_distance"]) {
                rhs.MaxDistance = node["max_distance"].as<float>();
            }
            return true;
        }
    };
}
