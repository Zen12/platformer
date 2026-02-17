#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "../fsm/node/action/action.hpp"
#include "../fsm/node/action/action_serialiazion.hpp"
#include "../fsm/engine_context.hpp"
#include <yaml-cpp/yaml.h>

class FsmActionRegistry final {
public:
    using DeserializerFn = std::function<std::unique_ptr<ActionSerialization>(const YAML::Node&)>;
    using BuilderFn = std::function<std::unique_ptr<Action>(const ActionSerialization*, const EngineContext&)>;

    void Register(const std::string& typeName, DeserializerFn deserializer, BuilderFn builder) {
        _deserializers[typeName] = std::move(deserializer);
        _builders[typeName] = std::move(builder);
    }

    [[nodiscard]] std::unique_ptr<ActionSerialization> Deserialize(const std::string& typeName, const YAML::Node& node) const {
        auto it = _deserializers.find(typeName);
        if (it == _deserializers.end()) return nullptr;
        auto result = it->second(node);
        if (result) result->Type = typeName;
        return result;
    }

    [[nodiscard]] std::unique_ptr<Action> Build(const std::string& typeName, const ActionSerialization* ser, const EngineContext& ctx) const {
        auto it = _builders.find(typeName);
        if (it == _builders.end()) return nullptr;
        return it->second(ser, ctx);
    }

    [[nodiscard]] bool HasType(const std::string& typeName) const {
        return _deserializers.find(typeName) != _deserializers.end();
    }

private:
    std::unordered_map<std::string, DeserializerFn> _deserializers;
    std::unordered_map<std::string, BuilderFn> _builders;
};
