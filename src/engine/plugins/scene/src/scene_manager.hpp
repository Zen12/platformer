#pragma once

#include "scene.hpp"
#include "scene_asset.hpp"
#include "prefab_asset.hpp"
#include "material/shader.hpp"
#include "material/material.hpp"
#include "asset_manager.hpp"
#include "input_system.hpp"
#include "guid.hpp"

#include "esc/esc.hpp"
#include "audio_manager.hpp"

#include <RmlUi/Core.h>


class SceneManager {

    std::shared_ptr<Scene> _scene;
    std::weak_ptr<AssetManager> _assetManager;
    std::weak_ptr<Window> _window;
    std::weak_ptr<InputSystem> _inputSystem;
    std::weak_ptr<AudioManager> _audioManager;
    std::shared_ptr<ResourceCache> _resourceCache;

    std::shared_ptr<RenderRepository> _renderRepository{};

    std::unique_ptr<EscSystem> _escSystem{};


public:
    SceneManager(
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<InputSystem> &inputSystem,
        const std::shared_ptr<ResourceCache> &resourceCache) {
        _assetManager = assetManager;
        _window = window;
        _inputSystem = inputSystem;
        _resourceCache = resourceCache;
        _renderRepository = std::make_shared<RenderRepository>();
    }

    void SetAudioManager(const std::weak_ptr<AudioManager>& audioManager) {
        _audioManager = audioManager;
    }

    [[nodiscard]] std::weak_ptr<AudioManager> GetAudioManager() const noexcept {
        return _audioManager;
    }

    void LoadScene(const SceneAsset& serialization);
    void LoadSceneByGuid(const Guid& sceneGuid);
    void LoadEntities(const std::vector<EntitySerialization> &serialization);
    void UnloadScene();

    void Update();

    [[nodiscard]] bool IsRequestToLoadScene() const;

    void LoadRequestedScene();

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const Guid& guid) const;

    [[nodiscard]] std::shared_ptr<UiPage> GetUiPage(const Guid& guid) const;

    [[nodiscard]] std::shared_ptr<RenderRepository> GetRenderRepository() const {
        return _renderRepository;
    }

    void ClearRenderRepository() const {
        _renderRepository->Clear();
    }

    [[nodiscard]] std::shared_ptr<Mesh> GetMesh(const Guid& guid) const {
        return _resourceCache->GetMesh(guid);
    }

    [[nodiscard]] std::shared_ptr<Shader> GetShader(const Guid &vertexGuid, const Guid &fragmentGuid) const;

    [[nodiscard]] std::shared_ptr<Scene> GetScene() const {
        return _scene;
    }

    [[nodiscard]] std::shared_ptr<ResourceCache> GetResourceCache() const {
        return _resourceCache;
    }

private:


    [[nodiscard]] std::shared_ptr<Texture> GetTexture(const Guid& guid) const;

    [[nodiscard]] std::shared_ptr<Font> GetFont(const Guid& guid) const;


};


