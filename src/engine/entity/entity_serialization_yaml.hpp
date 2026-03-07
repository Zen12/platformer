#pragma once

#include "entity_serialization.hpp"
#include "component_registry.hpp"

namespace YAML {
    template <>
    struct convert<EntitySerialization>
    {
        static const ComponentRegistry* s_componentRegistry;

        static bool decode(const Node &node, EntitySerialization &rhs)
        {
            if (node["creator"]) {
                rhs.Creator = node["creator"].as<std::string>();
            }
            if (node["tag"]) {
                rhs.Tag = node["tag"].as<std::string>();
            }
            if (node["guid"]) {
                rhs.Guid = node["guid"].as<std::string>();
            }
            if (node["layer"]) {
                rhs.Layer = node["layer"].as<int>();
            }

            if (node["components"]) {
                for (const auto& comp : node["components"]) {
                    rhs.Components.push_back(DecodeComponents(comp));
                }
            }

            return true;
        }

        static std::unique_ptr<ComponentSerialization> DecodeComponents(const YAML::Node& node) {

            const auto type = node["type"].as<std::string>();

            if (s_componentRegistry && s_componentRegistry->HasType(type)) {
                auto result = s_componentRegistry->Deserialize(type, node);
                if (result) result->ComponentType = type;
                return result;
            }

            throw std::runtime_error("Unknown component type: " + type);
        }

    };

    inline const ComponentRegistry* convert<EntitySerialization>::s_componentRegistry = nullptr;
}
