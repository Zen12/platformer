#pragma once
#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>

class EngineContext final {
    std::unordered_map<std::string, std::any> _services;

public:
    template<typename T>
    void Register(const std::string& name, std::shared_ptr<T> service) {
        _services[name] = std::move(service);
    }

    template<typename T>
    [[nodiscard]] std::shared_ptr<T> Get(const std::string& name) const {
        auto it = _services.find(name);
        if (it == _services.end()) return nullptr;
        return std::any_cast<std::shared_ptr<T>>(it->second);
    }

    template<typename T>
    [[nodiscard]] std::weak_ptr<T> GetWeak(const std::string& name) const {
        return Get<T>(name);
    }
};
