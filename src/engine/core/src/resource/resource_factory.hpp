#pragma once
#include "resource_cache.hpp"
#include "resource_key.hpp"
#include "asset_manager.hpp"
#include <any>
#include <typeindex>
#include <functional>
#include <unordered_map>
#include <stdexcept>

class ResourceFactory {
private:
    std::shared_ptr<ResourceCache> _cache;
    std::weak_ptr<AssetManager> _assetManager;
    std::unordered_map<std::type_index, std::any> _loaders;

public:
    explicit ResourceFactory(const std::weak_ptr<AssetManager>& assetManager)
        : _cache(std::make_shared<ResourceCache>()), _assetManager(assetManager) {}

    [[nodiscard]] std::weak_ptr<AssetManager> GetAssetManager() const noexcept {
        return _assetManager;
    }

    [[nodiscard]] std::shared_ptr<ResourceCache> GetCache() const noexcept {
        return _cache;
    }

    template <typename T, typename F>
    void RegisterLoader(F&& loader) {
        using KeyType = typename ResourceKey<T>::Type;
        using FnType = std::function<std::shared_ptr<T>(ResourceFactory&, const KeyType&)>;
        _loaders[std::type_index(typeid(T))] = FnType(std::forward<F>(loader));
    }

    template <typename T>
    [[nodiscard]] std::shared_ptr<T> Get(const typename ResourceKey<T>::Type& key) {
        auto it = _loaders.find(std::type_index(typeid(T)));
        if (it == _loaders.end()) {
            return {};
        }
        using KeyType = typename ResourceKey<T>::Type;
        using FnType = std::function<std::shared_ptr<T>(ResourceFactory&, const KeyType&)>;
        const auto& fn = std::any_cast<const FnType&>(it->second);
        return fn(*this, key);
    }

    template <typename T>
    [[nodiscard]] T GetValue(const Guid& guid) const { return _cache->GetValue<T>(guid); }

    template <typename T>
    void Register(const Guid& guid, std::shared_ptr<T> resource) { _cache->Register(guid, std::move(resource)); }

    template <typename T>
    void RegisterValue(const Guid& guid, T value) { _cache->RegisterValue(guid, std::move(value)); }
};
