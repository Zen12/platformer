#pragma once

#include "scene.hpp"
#include "scene_asset.hpp"
#include "../prefab/prefab_asset.hpp"
#include "../renderer/material/shader.hpp"
#include "../renderer/material/material.hpp"
#include "../asset/asset_manager.hpp"
#include "../system/input_system.hpp"

#include "../esc/esc.hpp"

#include <RmlUi/Core.h>


class SceneManager {

    std::shared_ptr<Scene> _scene;
    std::weak_ptr<AssetManager> _assetManager;
    std::weak_ptr<Window> _window;
    std::weak_ptr<InputSystem> _inputSystem;

    std::shared_ptr<RenderRepository> _renderRepository{};

    std::unique_ptr<EscSystem> _escSystem{};


public:
    SceneManager(
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<InputSystem> &inputSystem) {
        _assetManager = assetManager;
        _window = window;
        _inputSystem = inputSystem;
        _renderRepository = std::make_shared<RenderRepository>();
    }

    void LoadScene(const SceneAsset& serialization);
    void LoadSceneByGuid(const std::string& sceneGuid);
    void LoadEntities(const std::vector<EntitySerialization> &serialization);
    void UnloadScene();

    void Update();

    [[nodiscard]] bool IsRequestToLoadScene() const;

    void LoadRequestedScene();

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const std::string& guid) const;

    [[nodiscard]] std::shared_ptr<UiPage> GetUiPage(const std::string& guid) const;

    [[nodiscard]] std::shared_ptr<RenderRepository> GetRenderRepository() const {
        return _renderRepository;
    }

    void ClearRenderRepository() const {
        _renderRepository->Clear();
    }

    [[nodiscard]] std::shared_ptr<Mesh> GetMesh(const std::string& guid) const {
        if (_scene) {
            return _scene->GetMesh(guid);
        }
        return nullptr;
    }

    [[nodiscard]] std::shared_ptr<Shader> GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) const;

    [[nodiscard]] std::shared_ptr<Scene> GetScene() const {
        return _scene;
    }

private:


    [[nodiscard]] std::shared_ptr<Texture> GetTexture(const std::string& guid) const;

    [[nodiscard]] std::shared_ptr<Font> GetFont(const std::string& guid) const;


};


