
#pragma once
#include "condition_serialization.hpp"
#include "core_types/bool_check_condition_serialization_yaml.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<ConditionSerialization> {

        template<typename T>
        static std::unique_ptr<T> Parse(const YAML::Node &data) {
            static_assert(std::is_base_of_v<ConditionSerialization, T>,
                      "T must derive from ConditionSerialization");

            const auto condition = data.as<T>();
            return std::make_unique<T>(condition);
        }

        static std::unique_ptr<ConditionSerialization> DecodeCondition(const YAML::Node& node) {
            const auto type = node["type"].as<std::string>();

            using ConditionDeserializer = std::function<std::unique_ptr<ConditionSerialization>(const YAML::Node&)>;
            static const std::unordered_map<std::string, ConditionDeserializer> pairs = {
                { "bool_check", [](const YAML::Node& n){ return Parse<BoolCheckConditionSerialization>(n); } }
            };

            if (const auto it = pairs.find(type); it != pairs.end()) {
                return it->second(node);
            }

            return nullptr;
        }

        static bool decode(const Node &node, ConditionSerialization &rhs) {
            rhs.Type = node["type"].as<std::string>();
            rhs.Guid = node["guid"].as<std::string>();
            return true;
        }
    };
}
