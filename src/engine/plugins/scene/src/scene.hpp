#pragma once
#include <optional>
#include <unordered_map>
#include <vector>

#include "resource_cache.hpp"
#include "input_system.hpp"
#include "entt/entity/registry.hpp"
#include "navigation_manager.hpp"
#include "skybox/skybox_renderer.hpp"
#include "guid.hpp"

class AudioManager;

class Scene {
private:
    std::shared_ptr<ResourceCache> _resourceCache;

    std::weak_ptr<Window> _window;
    std::weak_ptr<AssetManager> _assetManager;
    std::weak_ptr<InputSystem> _inputSystem;
    std::weak_ptr<AudioManager> _audioManager;

    std::shared_ptr<entt::registry> _entityRegistry{};
    std::shared_ptr<NavigationManager> _navigationManager{};
    std::unique_ptr<SkyboxRenderer> _skyboxRenderer{};

    Guid _requestToLoadScene{};

public:

    Scene(
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<InputSystem> &inputSystem,
        const std::shared_ptr<ResourceCache> &resourceCache)
        :  _resourceCache(resourceCache),
           _window(window), _assetManager(assetManager),_inputSystem(inputSystem)

    {

        _entityRegistry = std::make_shared<entt::registry>();
        _navigationManager = std::make_shared<NavigationManager>();
    }


    [[nodiscard]] std::weak_ptr<AssetManager> GetAssetManager() const noexcept;

    [[nodiscard]] std::weak_ptr<InputSystem> GetInputSystem() const noexcept;

    [[nodiscard]] std::weak_ptr<Window> GetWindow() const noexcept;

    void SetAudioManager(const std::weak_ptr<AudioManager>& audioManager) noexcept {
        _audioManager = audioManager;
    }

    [[nodiscard]] std::weak_ptr<AudioManager> GetAudioManager() const noexcept {
        return _audioManager;
    }

    [[nodiscard]] std::shared_ptr<entt::registry> GetEntityRegistry() const noexcept {
        return _entityRegistry;
    }

    [[nodiscard]] std::shared_ptr<NavigationManager> GetNavigationManager() const noexcept {
        return _navigationManager;
    }

    [[nodiscard]] SkyboxRenderer* GetSkyboxRenderer() const noexcept {
        return _skyboxRenderer.get();
    }

    void InitializeSkybox(const Guid& materialGuid) {
        if (materialGuid.IsEmpty()) {
            _skyboxRenderer.reset();
            return;
        }
        _skyboxRenderer = std::make_unique<SkyboxRenderer>();
        _skyboxRenderer->Initialize(materialGuid);
    }

    [[nodiscard]] std::shared_ptr<ResourceCache> GetResourceCache() const noexcept {
        return _resourceCache;
    }

    [[nodiscard]] std::shared_ptr<Shader> GetShader(const Guid &vertexGuid, const Guid &fragmentGuid) {
        return _resourceCache->GetShader(vertexGuid, fragmentGuid);
    }

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const Guid &guid) {
        return _resourceCache->GetMaterial(guid);
    }

    std::shared_ptr<Mesh> GetMesh(const Guid &guid) {
        return _resourceCache->GetMesh(guid);
    }

    [[nodiscard]] std::shared_ptr<Texture> GetTexture(const Guid &guid) {
        return _resourceCache->GetTexture(guid);
    }

    [[nodiscard]] std::shared_ptr<Font> GetFont(const Guid &guid) {
        return _resourceCache->GetFont(guid);
    }

    [[nodiscard]] std::shared_ptr<UiPage> GetUiPage(const Guid &guid) {
        return _resourceCache->GetUiPage(guid);
    }

    [[nodiscard]] std::shared_ptr<AnimationData> GetAnimation(const Guid &guid) {
        return _resourceCache->GetAnimation(guid);
    }

    [[nodiscard]] std::vector<std::string> GetMeshBoneNames(const Guid &guid) const {
        return _resourceCache->GetMeshBoneNames(guid);
    }

    [[nodiscard]] std::vector<glm::mat4> GetMeshBoneOffsets(const Guid &guid) const noexcept {
        return _resourceCache->GetMeshBoneOffsets(guid);
    }

    [[nodiscard]] std::vector<int> GetMeshBoneParents(const Guid &guid) const noexcept {
        return _resourceCache->GetMeshBoneParents(guid);
    }

    [[nodiscard]] Bounds GetMeshBounds(const Guid &guid) const noexcept {
        return _resourceCache->GetMeshBounds(guid);
    }

    void RequestToLoadScene(const Guid &sceneGuid) noexcept {
        _requestToLoadScene = sceneGuid;
    }

    [[nodiscard]] Guid GetLoadSceneRequestGuid() const noexcept {
        return _requestToLoadScene;
    }

    void RegisterMesh(const Guid &guid, std::shared_ptr<Mesh> mesh) noexcept {
        _resourceCache->RegisterMesh(guid, std::move(mesh));
    }
};
