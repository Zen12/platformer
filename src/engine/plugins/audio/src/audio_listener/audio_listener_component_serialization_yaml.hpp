#pragma once
#include "audio_listener_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<AudioListenerComponentSerialization> {
        static bool decode([[maybe_unused]] const Node& node,
                          [[maybe_unused]] AudioListenerComponentSerialization& rhs) {
            // AudioListener is a marker component, nothing to decode
            return true;
        }
    };
}
