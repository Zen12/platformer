#pragma once
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
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

    template<typename TSerialization, typename F>
    void Register(const std::string& typeName, F&& emplacer) {
        static_assert(std::is_base_of_v<ComponentSerialization, TSerialization>);
        Register(typeName,
            [](const YAML::Node& n) -> std::unique_ptr<ComponentSerialization> {
                auto s = n.as<TSerialization>();
                return std::make_unique<TSerialization>(std::move(s));
            },
            [e = std::forward<F>(emplacer)](entt::registry& reg, entt::entity entity, const ComponentSerialization* ser) {
                const auto* s = dynamic_cast<const TSerialization*>(ser);
                if (!s) return;
                e(reg, entity, *s);
            }
        );
    }

    template<typename TSerialization, typename TComponent, typename F>
    void Register(const std::string& typeName, F&& converter) {
        static_assert(std::is_base_of_v<ComponentSerialization, TSerialization>);
        Register(typeName,
            [](const YAML::Node& n) -> std::unique_ptr<ComponentSerialization> {
                auto s = n.as<TSerialization>();
                return std::make_unique<TSerialization>(std::move(s));
            },
            [c = std::forward<F>(converter)](entt::registry& reg, entt::entity entity, const ComponentSerialization* ser) {
                const auto* s = dynamic_cast<const TSerialization*>(ser);
                if (!s) return;
                reg.emplace<TComponent>(entity, c(*s));
            }
        );
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
