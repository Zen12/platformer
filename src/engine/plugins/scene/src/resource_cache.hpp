#pragma once
#include "typed_cache.hpp"
#include <vector>
#include <memory>
#include <any>
#include <typeindex>
#include <unordered_map>

#include "material/shader.hpp"
#include "material/material.hpp"
#include "mesh/mesh.hpp"
#include "bounds.hpp"
#include "asset_manager.hpp"
#include "texture/texture_asset_loader.hpp"
#include "mesh/mesh_asset_loader.hpp"
#include "font_loader.hpp"
#include "ui/ui_page.hpp"
#include "material/material_asset.hpp"
#include "material/material_asset_yaml.hpp"
#include "ui/ui_page_asset.hpp"
#include "ui/ui_page_asset_yaml.hpp"
#include "animation/animation_data.hpp"
#include "animation/animation_asset_loader.hpp"
#include "guid.hpp"

struct FsmAsset;

class ResourceCache {
private:
    std::unordered_map<std::type_index, std::any> _caches;
    std::unordered_map<std::type_index, std::any> _valueCaches;

    std::weak_ptr<AssetManager> _assetManager;

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
    explicit ResourceCache(const std::weak_ptr<AssetManager>& assetManager)
        : _assetManager(assetManager) {}

    [[nodiscard]] std::weak_ptr<AssetManager> GetAssetManager() const noexcept {
        return _assetManager;
    }

    template <typename T>
    [[nodiscard]] std::shared_ptr<T> Get(const Guid& guid) {
        return Cache<T>().Get(guid);
    }

    template <typename T>
    void Register(const Guid& guid, std::shared_ptr<T> resource) {
        Cache<T>().Store(guid, std::move(resource));
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

    [[nodiscard]] std::shared_ptr<Shader> GetShader(const Guid& vertexGuid, const Guid& fragmentGuid);

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const Guid& guid);

    [[nodiscard]] std::shared_ptr<Mesh> GetMesh(const Guid& guid);

    [[nodiscard]] std::shared_ptr<Texture> GetTexture(const Guid& guid);

    [[nodiscard]] std::shared_ptr<Font> GetFont(const Guid& guid);

    [[nodiscard]] std::shared_ptr<UiPage> GetUiPage(const Guid& guid);

    [[nodiscard]] std::shared_ptr<AnimationData> GetAnimation(const Guid& guid);

    [[nodiscard]] std::shared_ptr<FsmAsset> GetFsmAsset(const Guid& guid);
};
