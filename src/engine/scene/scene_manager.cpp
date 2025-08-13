#include "scene_manager.hpp"

#include "../asset/factories/component_factory.hpp"
#include "../gameplay/character_controller.hpp"


template< typename TSerialization, typename TFactory>
void SceneManager::AddComponent(std::weak_ptr<Entity> e, TSerialization *serialization) const {

    static_assert(std::is_base_of_v<ComponentSerialization, TSerialization>,
          "TSerialization must inherit from ComponentSerialization");

    static_assert(std::is_base_of_v<BaseComponentFactory, TFactory>,
          "TFactory must inherit from BaseComponentFactory");

    auto cameraFactory = TFactory{};
    cameraFactory.SetScene(_scene);
    cameraFactory.SetEntity(e);
    cameraFactory.AddComponent(e, *serialization);
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


void SceneManager::LoadScene(const SceneAsset &scene) const {
    PROFILE_SCOPE("Loading of scene " + scene.Name);

    if (const auto assetManager = _scene->GetAssetManager().lock()) {
        std::vector<std::shared_ptr<Entity>> entities;

        for (const auto &entity : scene.Entities) {
            const auto newEntity = std::make_shared<Entity>();
            newEntity->SetId(entity.Guid);
            newEntity->SetTag(entity.Tag);

            entities.push_back(newEntity);
            _scene->AddEntity(newEntity);
        }

        for (size_t i = 0; i < scene.Entities.size(); i++) {
            auto &entity = scene.Entities[i];
            auto &newEntity = entities[i];

            for (const auto &comp : entity.Components)
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
                    CharacterControllerComponentSerialization, CharacterControllerFactory
                >(comp.get(), std::weak_ptr<Entity>(newEntity))) {
                    std::cerr << "can't add component" << std::endl;
                }
            }
        }

        const auto sceneCamera = _scene->FindByTag("main_camera");

        if (const auto camera = sceneCamera.lock()) {
            _scene->GetRenderPipeline().lock()->UpdateCamera(
      camera->GetComponent<CameraComponent>(),
      camera->GetComponent<Transform>());
        }
    }
}

void SceneManager::UnLoadAll() const {
    _scene->UnLoadAll();
}

std::weak_ptr<Entity> SceneManager::GetEntityById(const std::string &id) const {
    return _scene->GetEntityById(id);
}

void SceneManager::Update(const float &deltaTime) const {
    return _scene->Update(deltaTime);
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

std::shared_ptr<Sprite> SceneManager::GetSprite(const std::string &guid) const {
    return _scene->GetSprite(guid);
}

std::shared_ptr<Font> SceneManager::GetFont(const std::string &guid) const {
   return _scene->GetFont(guid);
}

std::shared_ptr<Entity> SceneManager::GetEntity(const std::string &id) const {
    return _scene->GetEntity(id);
}
