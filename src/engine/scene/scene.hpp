#pragma once
#include <unordered_map>
#include <vector>

#include "../renderer/material/shader.hpp"
#include "../renderer/material/material.hpp"
#include "../renderer/mesh/mesh.hpp"
#include "../asset/asset_manager.hpp"
#include "../renderer/texture/texture_asset_loader.hpp"
#include "../ui/text/font_loader.hpp"
#include "../debug/debug.hpp"
#include "../system/input_system.hpp"
#include "entt/entity/registry.hpp"


struct PrefabInstantiateData {
    std::string Id{};
    std::string Creator{};
    glm::vec3 Position{glm::vec3(1)};
    glm::vec3 Rotation{glm::vec3(0)};
    glm::vec3 Scale{glm::vec3(1)};
};

class Scene {
private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> _shaders;
    std::unordered_map<std::string, std::shared_ptr<Material>> _materials;
    std::unordered_map<std::string, std::shared_ptr<Texture>> _textures;
    std::unordered_map<std::string, std::shared_ptr<Font>> _fonts;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> _meshes;

    std::weak_ptr<Window> _window;
    std::weak_ptr<AssetManager> _assetManager;
    std::weak_ptr<InputSystem> _inputSystem;

    std::shared_ptr<entt::registry> _entityRegistry{};


    std::string _requestToLoadScene{};

public:

    Scene(
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<InputSystem> &inputSystem)
        :  _window(window), _assetManager(assetManager),_inputSystem(inputSystem)

    {

        _entityRegistry = std::make_shared<entt::registry>();
    }


    [[nodiscard]] std::weak_ptr<AssetManager> GetAssetManager() const noexcept;

    [[nodiscard]] std::weak_ptr<InputSystem> GetInputSystem() const noexcept;

    [[nodiscard]] std::weak_ptr<Window> GetWindow() const noexcept;

    [[nodiscard]] std::shared_ptr<entt::registry> GetEntityRegistry() const noexcept {
        return _entityRegistry;
    }

    [[nodiscard]] std::shared_ptr<Shader> GetShader(const std::string &vertexGuid, const std::string &fragmentGuid);

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const std::string &guid);

    std::shared_ptr<Mesh> GetMesh(const std::string &guid);

    [[nodiscard]] std::shared_ptr<Texture> GetTexture(const std::string &guid);

    [[nodiscard]] std::shared_ptr<Font> GetFont(const std::string &guid);

    void RequestToLoadScene(const std::string &sceneGuid) noexcept {
        _requestToLoadScene = sceneGuid;
    }

    [[nodiscard]] std::string GetLoadSceneRequestGuid() const noexcept {
        return _requestToLoadScene;
    }
};
