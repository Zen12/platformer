#pragma once
#include "combat_state_component_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<CombatStateComponentSerialization> {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] CombatStateComponentSerialization &rhs) {
            return true;
        }
    };
}
