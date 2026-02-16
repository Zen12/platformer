#pragma once

#include <yaml-cpp/yaml.h>
#include "guid.hpp"

namespace YAML {
    template <>
    struct convert<Guid> {
        static bool decode(const Node& node, Guid& rhs) {
            if (!node.IsScalar()) {
                return false;
            }
            rhs = Guid::FromString(node.as<std::string>());
            return true;
        }
    };
}
