#pragma once
#include "fsm_animation_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<FsmAnimationComponentSerialization> {
        static bool decode(const Node &node, FsmAnimationComponentSerialization &rhs) {
            if (node["fsm_guid"]) {
                rhs.FsmGuid = node["fsm_guid"].as<std::string>();
            }
            if (node["loop"]) {
                rhs.Loop = node["loop"].as<bool>();
            }
            return true;
        }
    };
}
