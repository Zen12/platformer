#pragma once
#include <unordered_map>
#include <memory>
#include "guid.hpp"

template <typename T>
class TypedCache {
private:
    std::unordered_map<Guid, std::shared_ptr<T>> _map{};

public:
    [[nodiscard]] bool Contains(const Guid& guid) const {
        return _map.find(guid) != _map.end();
    }

    [[nodiscard]] std::shared_ptr<T> Get(const Guid& guid) const {
        if (const auto it = _map.find(guid); it != _map.end()) {
            return it->second;
        }
        return nullptr;
    }

    void Store(const Guid& guid, std::shared_ptr<T> resource) {
        _map[guid] = std::move(resource);
    }

    template <typename Loader>
    [[nodiscard]] std::shared_ptr<T> GetOrLoad(const Guid& guid, Loader&& loader) {
        if (const auto it = _map.find(guid); it != _map.end()) {
            return it->second;
        }
        auto resource = loader();
        _map[guid] = resource;
        return resource;
    }
};

template <typename T>
class ValueCache {
private:
    std::unordered_map<Guid, T> _map{};

public:
    [[nodiscard]] bool Contains(const Guid& guid) const {
        return _map.find(guid) != _map.end();
    }

    [[nodiscard]] T Get(const Guid& guid) const {
        if (const auto it = _map.find(guid); it != _map.end()) {
            return it->second;
        }
        return T{};
    }

    void Store(const Guid& guid, T value) {
        _map[guid] = std::move(value);
    }
};
