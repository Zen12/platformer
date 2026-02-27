#pragma once
#include "example_condition_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template<>
    struct convert<ExampleAlwaysPassConditionSerialization> {
        static bool decode([[maybe_unused]] const Node& node,
                           [[maybe_unused]] ExampleAlwaysPassConditionSerialization& rhs) {
            return true;
        }
    };
}
