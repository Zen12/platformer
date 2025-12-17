#pragma once
#include "trigger_setter_button_listener_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<TriggerSetterButtonListenerActionSerialization>
    {
        static bool decode(const Node &node, TriggerSetterButtonListenerActionSerialization &rhs)
        {
            rhs.ButtonId = node["button_id"].as<std::string>();
            rhs.TriggerName = node["trigger_name"].as<std::string>();
            return true;
        }
    };
}
