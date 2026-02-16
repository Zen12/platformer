#pragma once
#include <optional>
#include <unordered_map>
#include <vector>

#include "../renderer/material/shader.hpp"
#include "../renderer/material/material.hpp"
#include "../renderer/mesh/mesh.hpp"
#include "../renderer/bounds.hpp"
#include "../asset/asset_manager.hpp"
#include "../renderer/texture/texture_asset_loader.hpp"
#include "../renderer/mesh/mesh_asset_loader.hpp"
#include "../font/font_loader.hpp"
#include "../renderer/ui/ui_page.hpp"
#include "../system/input_system.hpp"
#include "entt/entity/registry.hpp"
#include "../renderer/material/material_asset.hpp"
#include "../renderer/material/material_asset_yaml.hpp"
#include "../renderer/ui/ui_page.hpp"
#include "../renderer/ui/ui_page_asset.hpp"
#include "../renderer/ui/ui_page_asset_yaml.hpp"
#include "../renderer/animation/animation_data.hpp"
#include "../renderer/animation/animation_asset_loader.hpp"
#include "../navigation/navigation_manager.hpp"
#include "../renderer/skybox/skybox_renderer.hpp"
#include "guid.hpp"

class AudioManager;

class Scene {
private:
    std::unordered_map<Guid, std::shared_ptr<Shader>> _shaders{};
    std::unordered_map<Guid, std::shared_ptr<Material>> _materials{};
    std::unordered_map<Guid, std::shared_ptr<UiPage>> _uiPages{};
    std::unordered_map<Guid, std::shared_ptr<Texture>> _textures{};
    std::unordered_map<Guid, std::shared_ptr<Font>> _fonts{};
    std::unordered_map<Guid, std::shared_ptr<Mesh>> _meshes{};
    std::unordered_map<Guid, std::shared_ptr<AnimationData>> _animations{};
    std::unordered_map<Guid, std::vector<std::string>> _meshBoneNames{};
    std::unordered_map<Guid, std::vector<glm::mat4>> _meshBoneOffsets{};
    std::unordered_map<Guid, std::vector<int>> _meshBoneParents{};
    std::unordered_map<Guid, Bounds> _meshBounds{};

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
        const std::weak_ptr<InputSystem> &inputSystem)
        :  _window(window), _assetManager(assetManager),_inputSystem(inputSystem)

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

    [[nodiscard]] std::shared_ptr<Shader> GetShader(const Guid &vertexGuid, const Guid &fragmentGuid);

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const Guid &guid);

    std::shared_ptr<Mesh> GetMesh(const Guid &guid);

    [[nodiscard]] std::shared_ptr<Texture> GetTexture(const Guid &guid);

    [[nodiscard]] std::shared_ptr<Font> GetFont(const Guid &guid);

    [[nodiscard]] std::shared_ptr<UiPage> GetUiPage(const Guid &guid);

    [[nodiscard]] std::shared_ptr<AnimationData> GetAnimation(const Guid &guid);

    [[nodiscard]] std::vector<std::string> GetMeshBoneNames(const Guid &guid) const;

    [[nodiscard]] std::vector<glm::mat4> GetMeshBoneOffsets(const Guid &guid) const noexcept;

    [[nodiscard]] std::vector<int> GetMeshBoneParents(const Guid &guid) const noexcept;

    [[nodiscard]] Bounds GetMeshBounds(const Guid &guid) const noexcept;

    void RequestToLoadScene(const Guid &sceneGuid) noexcept {
        _requestToLoadScene = sceneGuid;
    }

    [[nodiscard]] Guid GetLoadSceneRequestGuid() const noexcept {
        return _requestToLoadScene;
    }

    void RegisterMesh(const Guid &guid, std::shared_ptr<Mesh> mesh) noexcept {
        _meshes[guid] = std::move(mesh);
    }
};
