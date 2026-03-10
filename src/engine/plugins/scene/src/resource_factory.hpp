#pragma once
#include "resource_cache.hpp"
#include "resource_key.hpp"
#include "asset_manager.hpp"

class Material;
class Shader;
class Mesh;
class Texture;
class Font;
class UiPage;
class AnimationData;
struct FsmAsset;

class ResourceFactory {
private:
    std::shared_ptr<ResourceCache> _cache;
    std::weak_ptr<AssetManager> _assetManager;

public:
    explicit ResourceFactory(const std::weak_ptr<AssetManager>& assetManager)
        : _cache(std::make_shared<ResourceCache>()), _assetManager(assetManager) {}

    [[nodiscard]] std::weak_ptr<AssetManager> GetAssetManager() const noexcept {
        return _assetManager;
    }

    template <typename T>
    [[nodiscard]] std::shared_ptr<T> Get(const typename ResourceKey<T>::Type& key);

    template <typename T>
    [[nodiscard]] T GetValue(const Guid& guid) const { return _cache->GetValue<T>(guid); }

    template <typename T>
    void Register(const Guid& guid, std::shared_ptr<T> resource) { _cache->Register(guid, std::move(resource)); }

    template <typename T>
    void RegisterValue(const Guid& guid, T value) { _cache->RegisterValue(guid, std::move(value)); }
};

template <> std::shared_ptr<Shader> ResourceFactory::Get<Shader>(const std::tuple<Guid, Guid>& key);
template <> std::shared_ptr<Material> ResourceFactory::Get<Material>(const Guid& key);
template <> std::shared_ptr<Mesh> ResourceFactory::Get<Mesh>(const Guid& key);
template <> std::shared_ptr<Texture> ResourceFactory::Get<Texture>(const Guid& key);
template <> std::shared_ptr<Font> ResourceFactory::Get<Font>(const Guid& key);
template <> std::shared_ptr<UiPage> ResourceFactory::Get<UiPage>(const Guid& key);
template <> std::shared_ptr<AnimationData> ResourceFactory::Get<AnimationData>(const Guid& key);
template <> std::shared_ptr<FsmAsset> ResourceFactory::Get<FsmAsset>(const Guid& key);
