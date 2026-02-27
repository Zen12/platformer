#pragma once
#include "example_noop_action_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template<>
    struct convert<ExampleNoopActionSerialization> {
        static bool decode(const Node& node, ExampleNoopActionSerialization& rhs) {
            if (node["label"]) {
                rhs.Label = node["label"].as<std::string>();
            } else {
                rhs.Label = "example";
            }
            return true;
        }
    };
}
