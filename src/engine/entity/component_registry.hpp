#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "component_serialization.hpp"
#include "entt/entity/registry.hpp"
#include <yaml-cpp/yaml.h>

class ComponentRegistry final {
public:
    using DeserializerFn = std::function<std::unique_ptr<ComponentSerialization>(const YAML::Node&)>;
    using EmplacerFn = std::function<void(entt::registry&, entt::entity, const ComponentSerialization*)>;

    void Register(const std::string& typeName, DeserializerFn deserializer, EmplacerFn emplacer) {
        _deserializers[typeName] = std::move(deserializer);
        _emplacers[typeName] = std::move(emplacer);
    }

    [[nodiscard]] std::unique_ptr<ComponentSerialization> Deserialize(const std::string& typeName, const YAML::Node& node) const {
        auto it = _deserializers.find(typeName);
        if (it == _deserializers.end()) return nullptr;
        return it->second(node);
    }

    bool Emplace(const std::string& typeName, entt::registry& reg, entt::entity entity, const ComponentSerialization* ser) const {
        auto it = _emplacers.find(typeName);
        if (it == _emplacers.end()) return false;
        it->second(reg, entity, ser);
        return true;
    }

    [[nodiscard]] bool HasType(const std::string& typeName) const {
        return _deserializers.find(typeName) != _deserializers.end();
    }

private:
    std::unordered_map<std::string, DeserializerFn> _deserializers;
    std::unordered_map<std::string, EmplacerFn> _emplacers;
};
