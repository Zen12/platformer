#pragma once
#include "log_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<LogActionSerialization> {
        static bool decode(const Node &node, LogActionSerialization &rhs) {
            rhs.Message = node["message"].as<std::string>();
            return true;
        }
    };
}
