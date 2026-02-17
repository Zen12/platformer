#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "../fsm/condition/condition_base.hpp"
#include "../fsm/condition/condition_serialization.hpp"
#include "../fsm/engine_context.hpp"
#include <yaml-cpp/yaml.h>

class FsmConditionRegistry final {
public:
    using DeserializerFn = std::function<std::unique_ptr<ConditionSerialization>(const YAML::Node&)>;
    using BuilderFn = std::function<std::unique_ptr<ConditionBase>(const ConditionSerialization*, const EngineContext&)>;

    void Register(const std::string& typeName, DeserializerFn deserializer, BuilderFn builder) {
        _deserializers[typeName] = std::move(deserializer);
        _builders[typeName] = std::move(builder);
    }

    [[nodiscard]] std::unique_ptr<ConditionSerialization> Deserialize(const std::string& typeName, const YAML::Node& node) const {
        auto it = _deserializers.find(typeName);
        if (it == _deserializers.end()) return nullptr;
        return it->second(node);
    }

    [[nodiscard]] std::unique_ptr<ConditionBase> Build(const std::string& typeName, const ConditionSerialization* ser, const EngineContext& ctx) const {
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
