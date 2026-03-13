#pragma once
#include "typed_cache.hpp"
#include <memory>
#include <any>
#include <typeindex>
#include <unordered_map>
#include "guid.hpp"

class ResourceCache {
private:
    std::unordered_map<std::type_index, std::any> _caches;
    std::unordered_map<std::type_index, std::any> _valueCaches;

    template <typename T>
    TypedCache<T>& Cache() {
        auto key = std::type_index(typeid(T));
        auto it = _caches.find(key);
        if (it == _caches.end()) {
            _caches[key] = TypedCache<T>{};
            it = _caches.find(key);
        }
        return std::any_cast<TypedCache<T>&>(it->second);
    }

    template <typename T>
    ValueCache<T>& VCache() {
        auto key = std::type_index(typeid(T));
        auto it = _valueCaches.find(key);
        if (it == _valueCaches.end()) {
            _valueCaches[key] = ValueCache<T>{};
            it = _valueCaches.find(key);
        }
        return std::any_cast<ValueCache<T>&>(it->second);
    }

    template <typename T>
    const ValueCache<T>* VCacheConst() const {
        auto key = std::type_index(typeid(T));
        auto it = _valueCaches.find(key);
        if (it == _valueCaches.end()) {
            return nullptr;
        }
        return std::any_cast<const ValueCache<T>>(&it->second);
    }

public:
    template <typename T>
    [[nodiscard]] std::shared_ptr<T> Get(const Guid& guid) {
        return Cache<T>().Get(guid);
    }

    template <typename T>
    void Register(const Guid& guid, std::shared_ptr<T> resource) {
        Cache<T>().Store(guid, std::move(resource));
    }

    template <typename T, typename Loader>
    [[nodiscard]] std::shared_ptr<T> GetOrLoad(const Guid& guid, Loader&& loader) {
        return Cache<T>().GetOrLoad(guid, std::forward<Loader>(loader));
    }

    template <typename T>
    [[nodiscard]] T GetValue(const Guid& guid) const {
        if (const auto* cache = VCacheConst<T>()) {
            return cache->Get(guid);
        }
        return T{};
    }

    template <typename T>
    void RegisterValue(const Guid& guid, T value) {
        VCache<T>().Store(guid, std::move(value));
    }

    void Clear() {
        _caches.clear();
        _valueCaches.clear();
    }
};
