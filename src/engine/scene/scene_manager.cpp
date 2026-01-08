#include "scene_manager.hpp"
#include "scene_asset_yaml.hpp"

#include <GL/glew.h>
#include <fstream>
#include "../renderer/controller/ui_opengl_render_controller.hpp"
#include "../esc/transform/transform_component.hpp"
#include "../esc/transform/transform_component_serialization.hpp"
#include "../esc/mesh_renderer/mesh_renderer_component.hpp"
#include "../esc/mesh_renderer/mesh_renderer_component_serialization.hpp"

#define DEBUG_ENGINE_SCENE_MANAGER_PROFILE 0


void SceneManager::LoadScene(const SceneAsset &sceneAsset) {
#if DEBUG_ENGINE_SCENE_MANAGER_PROFILE
    PROFILE_SCOPE("Loading of scene " + sceneAsset.Name);
#endif

    if (const auto assetManager = _assetManager.lock()) {
        _scene = std::make_shared<Scene> (_window,_assetManager, _inputSystem);
        _scene->SetAudioManager(_audioManager);

        if (const auto navigationManager = _scene->GetNavigationManager()) {
            if (sceneAsset.Navmesh.has_value()) {
                const auto& navmesh = sceneAsset.Navmesh.value();
                navigationManager->Initialize(navmesh.Width, navmesh.Height,
                                            navmesh.CellSize, navmesh.Origin,
                                            navmesh.WalkabilityGrid, navmesh.MaxAgents);
            } else {
                navigationManager->Initialize(50, 50, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            }

        }

        if (sceneAsset.Skybox.has_value()) {
            _scene->InitializeSkybox(sceneAsset.Skybox->MaterialGuid);
        }

        LoadEntities(sceneAsset.Entities);
    }
}

void SceneManager::LoadSceneByGuid(const std::string& sceneGuid) {
    if (const auto assetManager = _assetManager.lock()) {
        const auto sceneAsset = assetManager->LoadAssetByGuid<SceneAsset>(sceneGuid);
        LoadScene(sceneAsset);
    }
}

void SceneManager::LoadEntities(const std::vector<EntitySerialization> &serialization) {

    _escSystem = std::make_unique<EscSystem>(_scene);

    _escSystem->LoadEntities(serialization);

    _escSystem->InitSystems(_renderRepository, _audioManager);
}

void SceneManager::UnloadScene() {
    if (_escSystem) {
        _escSystem.reset();
    }
    if (_scene) {
        _scene.reset();
    }
    if (_renderRepository) {
        _renderRepository->Clear();
    }
}

void SceneManager::Update() {
#if DEBUG_ENGINE_SCENE_MANAGER_PROFILE
    PROFILE_SCOPE("  SceneManager::Update");
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_escSystem) {
        _escSystem->Update(); // ATM single threaded
    }

    // here we should do rendering
}

std::shared_ptr<Shader> SceneManager::GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) const {
    if (_scene) {
        return _scene->GetShader(vertexGuid, fragmentGuid);
    }
    return nullptr;
}

std::shared_ptr<Material> SceneManager::GetMaterial(const std::string &guid) const {
    if (_scene) {
        return _scene->GetMaterial(guid);
    }
    return nullptr;
}

bool SceneManager::IsRequestToLoadScene() const {
    if (_scene) {
        return !_scene->GetLoadSceneRequestGuid().empty();
    }
    return false;
}

void SceneManager::LoadRequestedScene() {
    if (_scene && !_scene->GetLoadSceneRequestGuid().empty()) {
        if (const auto assetManager = _assetManager.lock()) {
            const auto scene = assetManager->LoadAssetByGuid<SceneAsset>(_scene->GetLoadSceneRequestGuid());
            LoadScene(scene);
        }
    }
}

std::shared_ptr<Texture> SceneManager::GetTexture(const std::string &guid) const {
    if (_scene) {
        return _scene->GetTexture(guid);
    }
    return nullptr;
}

std::shared_ptr<Font> SceneManager::GetFont(const std::string &guid) const {
    if (_scene) {
        return _scene->GetFont(guid);
    }
    return nullptr;
}

std::shared_ptr<UiPage> SceneManager::GetUiPage(const std::string &guid) const {
    if (_scene) {
        return _scene->GetUiPage(guid);
    }
    return nullptr;
}

