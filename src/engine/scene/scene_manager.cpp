#include "scene_manager.hpp"

#include "../game/character_controller/character_animation/character_animation_component_factory.hpp"
#include "../game/character_controller/character_animation/character_animation_component_serialization.hpp"
#include "../game/character_controller/movement/character_movement_component_factory.hpp"
#include "../game/character_controller/movement/character_movement_component_serialization.hpp"

#define DEBUG_ENGINE_SCENE_MANAGER_PROFILE 0


template< typename TSerialization, typename TFactory>
void SceneManager::AddComponent(std::weak_ptr<Entity> e, TSerialization *serialization) const {

    static_assert(std::is_base_of_v<ComponentSerialization, TSerialization>,
          "TSerialization must inherit from ComponentSerialization");

    static_assert(std::is_base_of_v<BaseComponentFactory, TFactory>,
          "TFactory must inherit from BaseComponentFactory");

    auto factory = TFactory{};
    factory.SetScene(_scene);
    factory.SetEntity(e);
    factory.AddComponent(e, *serialization);
}

template<typename TSerialization, typename TFactory>
bool SceneManager::TryToAddComponent(ComponentSerialization* component, std::weak_ptr<Entity> e) const {

    static_assert(std::is_base_of_v<ComponentSerialization, TSerialization>,
              "TSerialization must inherit from ComponentSerialization");

    static_assert(std::is_base_of_v<BaseComponentFactory, TFactory>,
      "TFactory must inherit from BaseComponentFactory");

    if (const auto componentSerialization = dynamic_cast<TSerialization *>(component)) {
        AddComponent<TSerialization, TFactory>(e, componentSerialization);
        return true;
    }

    return false;
}

template <typename TSerialization, typename TFactory, typename... Rest>
bool SceneManager::TryToAddComponents(ComponentSerialization* comp, std::weak_ptr<Entity> e) const {

    static_assert(std::is_base_of_v<ComponentSerialization, TSerialization>,
                  "TSerialization must inherit from ComponentSerialization");

    static_assert(std::is_base_of_v<BaseComponentFactory, TFactory>,
      "TFactory must inherit from BaseComponentFactory");

    if (TryToAddComponent<TSerialization, TFactory>(comp, e)) {
        return true;
    }
    if constexpr (sizeof...(Rest) > 0) {
        return TryToAddComponents<Rest...>(comp, e);
    }
    return false;
}


void SceneManager::LoadScene(const SceneAsset &sceneAsset) {
#ifndef NDEBUG
#if DEBUG_ENGINE_SCENE_MANAGER_PROFILE
    PROFILE_SCOPE("Loading of scene " + sceneAsset.Name);
#endif
#endif


    if (const auto assetManager = _assetManager.lock()) {
        _scene = std::make_shared<Scene> (_window,_assetManager, _inputSystem);

        LoadEntities(sceneAsset.Entities);

        const auto sceneCamera = _scene->FindByTag("main_camera");

        if (const auto camera = sceneCamera.lock()) {
            _scene->GetRenderPipeline().lock()->UpdateCamera(
      camera->GetComponent<CameraComponent>(),
      camera->GetComponent<Transform>());
        }
    }
}

void SceneManager::LoadEntities(const std::vector<EntitySerialization> &serialization) const {

    std::vector<std::weak_ptr<Entity>> entities;
    for (const auto &entitySerialization : serialization) {
        entities.push_back(_scene->CreateEntity(entitySerialization));
    }

    for (size_t i = 0; i < serialization.size(); i++) {
        const auto &entitySerialization = serialization[i];
        const auto &entityInstance = entities[i];

        for (const auto &comp : entitySerialization.Components)
        {
            if (!TryToAddComponents<
                CameraComponentSerialization, CameraComponentFactory,
                TransformComponentSerialization, TransformFactory,
                RectTransformComponentSerialization, RectTransformFactory,
                UiImageComponentSerialization, UiImageFactory,
                UiTextComponentSerialization, UiTextComponentFactory,
                SpriteRenderComponentSerialization, SpriteRendererFactory,
                SpineRenderComponentSerialization, SpineRendererFactory,
                Box2dColliderSerialization, BoxCollider2dFactory,
                Rigidbody2dSerialization, Rigidbody2dFactory,
                Light2dComponentSerialization, Light2dFactory,
                MeshRendererComponentSerialization, MeshRendererFactory,
                CharacterControllerComponentSerialization, CharacterControllerFactory,
                CharacterMovementComponentSerialization, CharacterMovementFactory,
                CharacterAnimationComponentSerialization, CharacterAnimationComponentFactory,
                AiControllerComponentSerialization, AiControllerFactory,
                ShowFpsComponentSerialization, ShowFpsComponentFactory,
                HealthComponentSerialization, HealthComponentFactory,
                PrefabSpawnerSerialization, PrefabSpawnerFactor,
                GridPrefabSpawnerSerialization, GridPrefabSpawnerFactor,
                GridSerialization, GridFactory,
                PathFinderSerialization, PathFinderFactory,
                ParticleEmitterSerialization, ParticleEmitterComponentFactory,
                SpineColliderSerialization, SpineColliderComponentFactory,
                HealthBarComponentSerialization, HealthBarComponentFactory,
                RectTransformFollowerSerialization, RectTransformFollowerFactory,
                DestroyWithCreatorComponentSerialization, DestroyWithCreatorComponentFactory,
                UiButtonComponentSerialization, UiButtonComponentFactory,
                IdleCharacterSerialization, IdleCharacterComponentFactor,
                OnClickSceneLoaderSerialization, OnClickSceneLoaderFactory,
                PathMoverComponentSerialization, PathMoverComponentFactor,
                GameStateData, GameStateFactory,
                TeamSerialization, TeamComponentFactory,
                UiButtonEffectSerialization, UiButtonEffectFactory
            >(comp.get(), std::weak_ptr<Entity>(entityInstance))) {
                std::cerr << "can't add component" << std::endl;
#ifndef NDEBUG
                exit(-1);
#endif
            }
        }
    }
}

std::weak_ptr<Entity> SceneManager::GetEntityById(const std::string &id) const {
    return _scene->GetEntityById(id);
}

void SceneManager::CreateRequestedPrefabs() const {

    std::vector<EntitySerialization> serializations{};

    if (const auto assetManager = _assetManager.lock()) {

        for (const auto& prefabRequest: _scene->PrefabRequestInstantiate) {
            auto prefabAsset = assetManager->LoadAssetByGuid<PrefabAsset>(prefabRequest.Id);
            prefabAsset.Obj.Guid = GuidGenerator::GenerateGuid();
            prefabAsset.Obj.Creator = prefabRequest.Creator;
            for (auto &component : prefabAsset.Obj.Components) {

                if (auto *tr = dynamic_cast<TransformComponentSerialization *>(component.get()); tr != nullptr) {
                    tr->position = prefabRequest.Position;
                    tr->rotation = prefabRequest.Rotation;
                    tr->scale = prefabRequest.Scale;
                }
            }


            serializations.push_back(prefabAsset.Obj);
        }
    }


    LoadEntities(serializations);
    _scene->PrefabRequestInstantiate.clear();
}

void SceneManager::Update(const float &deltaTime) {
#ifndef NDEBUG
#if DEBUG_ENGINE_SCENE_MANAGER_PROFILE
    PROFILE_SCOPE("  SceneManager::Update");
#endif
#endif

    _scene->RemovePendingEntities();
    CreateRequestedPrefabs();
    _scene->Update(deltaTime);
}

void SceneManager::Render(const float &deltaTime) const  {
#ifndef NDEBUG
#if DEBUG_ENGINE_SCENE_MANAGER_PROFILE
    PROFILE_SCOPE("  SceneManager::Render");
#endif
#endif

    _scene->Render(deltaTime);
}

std::shared_ptr<Shader> SceneManager::GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) const {
    return _scene->GetShader(vertexGuid, fragmentGuid);
}

std::shared_ptr<Material> SceneManager::GetMaterial(const std::string &guid) const {
    return _scene->GetMaterial(guid);
}

std::shared_ptr<SpineData> SceneManager::LoadSpineData(const SpineAsset& asset) const {
    return _scene->LoadSpineData(asset);
}


std::shared_ptr<SpineData> SceneManager::GetSpineData(const std::string &guid, const SpineAsset& asset) const {
    return _scene->GetSpineData(guid, asset);
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

std::shared_ptr<Sprite> SceneManager::GetSprite(const std::string &guid) const {
    return _scene->GetSprite(guid);
}

std::shared_ptr<Font> SceneManager::GetFont(const std::string &guid) const {
   return _scene->GetFont(guid);
}

std::shared_ptr<Entity> SceneManager::GetEntity(const std::string &id) const {
    return _scene->GetEntity(id);
}
