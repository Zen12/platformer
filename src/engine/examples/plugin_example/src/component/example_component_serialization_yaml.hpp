#pragma once
#include "example_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template<>
    struct convert<ExampleTagComponentSerialization> {
        static bool decode(const Node& node, ExampleTagComponentSerialization& rhs) {
            if (node["tag"]) {
                rhs.Tag = node["tag"].as<std::string>();
            }
            return true;
        }
    };
}
