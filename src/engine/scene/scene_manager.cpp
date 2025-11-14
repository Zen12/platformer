#include "scene_manager.hpp"
#include "scene_asset_yaml.hpp"

#include "../esc/tag/tag_component.hpp"

#define DEBUG_ENGINE_SCENE_MANAGER_PROFILE 0


void SceneManager::LoadScene(const SceneAsset &sceneAsset) {
#ifndef NDEBUG
#if DEBUG_ENGINE_SCENE_MANAGER_PROFILE
    PROFILE_SCOPE("Loading of scene " + sceneAsset.Name);
#endif
#endif


    if (const auto assetManager = _assetManager.lock()) {
        _scene = std::make_shared<Scene> (_window,_assetManager, _inputSystem);

        LoadEntities(sceneAsset.Entities);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void SceneManager::LoadEntities(const std::vector<EntitySerialization> &serialization) {

    _escSystem = std::make_unique<EscSystem>(_scene);

    _escSystem->LoadEntities(serialization);

    _escSystem->InitSystems();
}


void SceneManager::Update() {
#ifndef NDEBUG
#if DEBUG_ENGINE_SCENE_MANAGER_PROFILE
    PROFILE_SCOPE("  SceneManager::Update");
#endif
#endif
    glClear(GL_COLOR_BUFFER_BIT);

    _escSystem->Update(); // ATM single threaded

    // here we should do rendering
}

std::shared_ptr<Shader> SceneManager::GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) const {
    return _scene->GetShader(vertexGuid, fragmentGuid);
}

std::shared_ptr<Material> SceneManager::GetMaterial(const std::string &guid) const {
    return _scene->GetMaterial(guid);
}

bool SceneManager::IsRequestToLoadScene() const {
    return !_scene->GetLoadSceneRequestGuid().empty();
}

void SceneManager::LoadRequestedScene() {
    if (!_scene->GetLoadSceneRequestGuid().empty()) {
        if (const auto assetManager = _assetManager.lock()) {
            const auto scene = assetManager->LoadAssetByGuid<SceneAsset>(_scene->GetLoadSceneRequestGuid());
            LoadScene(scene);
        }
    }
}

std::shared_ptr<Texture> SceneManager::GetSprite(const std::string &guid) const {
    return _scene->GetTexture(guid);
}

std::shared_ptr<Font> SceneManager::GetFont(const std::string &guid) const {
   return _scene->GetFont(guid);
}
