#pragma once
#include "button_listener_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<ButtonListenerActionSerialization>
    {
        static bool decode(const Node &node, ButtonListenerActionSerialization &rhs)
        {
            rhs.ButtonId = node["button_id"].as<std::string>();
            return true;
        }
    };
}
