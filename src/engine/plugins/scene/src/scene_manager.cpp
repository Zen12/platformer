#include "scene_manager.hpp"
#include "scene_asset_yaml.hpp"

#include <GL/glew.h>
#include <fstream>
#include <glm/vec4.hpp>
#include "controller/ui_opengl_render_controller.hpp"
#include "plugins/core/src/transform/transform_component.hpp"
#include "plugins/core/src/transform/transform_component_serialization.hpp"
#include "plugins/core/src/mesh_renderer/mesh_renderer_component.hpp"
#include "plugins/core/src/mesh_renderer/mesh_renderer_component_serialization.hpp"
#include "navmesh_mesh_generator.hpp"
#include "profiler.hpp"


void SceneManager::LoadScene(const SceneAsset &sceneAsset) {
    PROFILE_SCOPE("LoadScene");

    if (const auto assetManager = _assetManager.lock()) {
        _scene = std::make_shared<Scene> (_window,_assetManager, _inputSystem, _resourceCache);
        _scene->SetAudioManager(_audioManager);
        _scene->SetActionRegistry(_actionRegistry);
        _scene->SetConditionRegistry(_conditionRegistry);
        _scene->SetEngineContext(_engineContext);

        if (const auto navigationManager = _scene->GetNavigationManager()) {
            if (sceneAsset.Navmesh.has_value()) {
                const auto& navmesh = sceneAsset.Navmesh.value();
                navigationManager->Initialize(navmesh.Width, navmesh.Height,
                                            navmesh.CellSize, navmesh.Origin,
                                            navmesh.WalkabilityGrid, navmesh.MaxAgents);

                if (const auto gridNavmesh = navigationManager->GetNavmesh()) {
                    auto navmeshMesh = NavmeshMeshGenerator::Generate(
                        *gridNavmesh,
                        navmesh.Origin.y,
                        navmesh.ElevationHeight
                    );

                    // Create entity for combined navmesh mesh (ground + walls + ramps)
                    if (navmeshMesh && navmesh.GroundMaterialGuid.has_value()) {
                        const Guid meshGuid = Guid::Generate();
                        _scene->RegisterMesh(meshGuid, navmeshMesh);

                        if (const auto registry = _scene->GetEntityRegistry()) {
                            const auto entity = registry->create();
                            registry->emplace<TransformComponentV2>(entity);
                            registry->emplace<MeshRendererComponent>(entity,
                                meshGuid,
                                navmesh.GroundMaterialGuid.value(),
                                glm::vec4(0.3f, 0.6f, 0.3f, 1.0f));
                        }
                    }
                }
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

void SceneManager::LoadSceneByGuid(const Guid& sceneGuid) {
    if (const auto assetManager = _assetManager.lock()) {
        const auto sceneAsset = assetManager->LoadAssetByGuid<SceneAsset>(sceneGuid);
        LoadScene(sceneAsset);
    }
}

void SceneManager::LoadEntities(const std::vector<EntitySerialization> &serialization) {

    _escSystem = std::make_unique<EscSystem>(_scene, _componentRegistry);

    _escSystem->LoadEntities(serialization);

    _escSystem->InitSystems(_renderRepository, _resourceCache, _audioManager, _systemRegistry);
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
    PROFILE_SCOPE("SceneManager::Update");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_escSystem) {
        _escSystem->Update(); // ATM single threaded
    }

    // here we should do rendering
}

std::shared_ptr<Shader> SceneManager::GetShader(const Guid &vertexGuid, const Guid &fragmentGuid) const {
    return _resourceCache->GetShader(vertexGuid, fragmentGuid);
}

std::shared_ptr<Material> SceneManager::GetMaterial(const Guid &guid) const {
    return _resourceCache->GetMaterial(guid);
}

bool SceneManager::IsRequestToLoadScene() const {
    if (_scene) {
        return !_scene->GetLoadSceneRequestGuid().IsEmpty();
    }
    return false;
}

void SceneManager::LoadRequestedScene() {
    if (_scene && !_scene->GetLoadSceneRequestGuid().IsEmpty()) {
        if (const auto assetManager = _assetManager.lock()) {
            const auto scene = assetManager->LoadAssetByGuid<SceneAsset>(_scene->GetLoadSceneRequestGuid());
            LoadScene(scene);
        }
    }
}

std::shared_ptr<Texture> SceneManager::GetTexture(const Guid &guid) const {
    return _resourceCache->GetTexture(guid);
}

std::shared_ptr<Font> SceneManager::GetFont(const Guid &guid) const {
    return _resourceCache->GetFont(guid);
}

std::shared_ptr<UiPage> SceneManager::GetUiPage(const Guid &guid) const {
    return _resourceCache->GetUiPage(guid);
}

