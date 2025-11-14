#include "scene.hpp"
#include "../renderer/material/material_asset.hpp"
#include "../renderer/material/material_asset_yaml.hpp"
#include <entt/entity/registry.hpp>
#include "../entity/destroy_request_component.hpp"

std::shared_ptr<RectTransformRoot> Scene::GetRoot() const {
    return _root;
}

std::weak_ptr<RenderPipeline> Scene::GetRenderPipeline() const noexcept {
    return _renderPipeline;
}

std::weak_ptr<AssetManager> Scene::GetAssetManager() const noexcept {
    return _assetManager;
}

std::weak_ptr<InputSystem> Scene::GetInputSystem() const noexcept {
    return _inputSystem;
}

std::weak_ptr<Window> Scene::GetWindow() const noexcept {
    return _window;
}

std::weak_ptr<Entity> Scene::CreateEntity(const EntitySerialization &entitySerialization) {
    const auto newEntity = std::make_shared<Entity>();
    newEntity->SetId(entitySerialization.Guid);
    newEntity->SetTag(entitySerialization.Tag);
    newEntity->SetLayer(entitySerialization.Layer);
    newEntity->SetSelf(newEntity);
    newEntity->SetCreator(entitySerialization.Creator);

    const auto entt = _entityRegistry->create();
    _entityRegistry->emplace<std::shared_ptr<Entity>>(entt, newEntity);
    return newEntity;
}

std::weak_ptr<Entity> Scene::GetEntityById(const std::string &id) const {
    const auto view = _entityRegistry->view<std::shared_ptr<Entity>>();

    for (const auto &entity: view) {
        if (const auto& en = view.get<std::shared_ptr<Entity>>(entity); en->GetId() == id) {
            return en;
        }
    }
    return {};
}

size_t Scene::GetEntityCount() const noexcept {
    const auto view = _entityRegistry->view<std::shared_ptr<Entity>>();
    return view.size();
}

void Scene::RemoveEntityById(const std::string &id) {
    _entitiesToRemove.push_back(id);
}

void Scene::Update(const float &deltaTime) {
    const auto view = _entityRegistry->view<std::shared_ptr<Entity>>();
    for (const auto entity : view) {
        auto& pos = view.get<std::shared_ptr<Entity>>(entity);
        pos->Update(deltaTime);
    }

    _uiRaycastSystem->UpdateState();

    glClear(GL_COLOR_BUFFER_BIT);
}

void Scene::RemovePendingEntities() {
    // legacy
    const auto view = _entityRegistry->view<std::shared_ptr<Entity>>();

    for (const auto & id: _entitiesToRemove) {
        const auto it = std::find_if(view.begin(), view.end(),
                                     [id, view](const entt::entity &entity) {
                                         return view.get<std::shared_ptr<Entity>>(entity)->GetId() == id;
                                     });


        if (it != view.end()) {
            _entityRegistry->destroy(*it);
        }
    }

    _entitiesToRemove.clear();

    // new
    const auto viewDestroy = _entityRegistry->view<DestroyRequestComponent>();
    for (const auto &entity: viewDestroy) {
        _entityRegistry->destroy(entity);
    }

}

std::shared_ptr<Shader> Scene::GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) {
    if (const auto assetManager = _assetManager.lock()) {

        if (_shaders.find(vertexGuid + fragmentGuid) == _shaders.end()) {
            const auto vertexSource = assetManager->LoadSourceByGuid<std::string>(vertexGuid);
            const auto fragmentSource = assetManager->LoadSourceByGuid<std::string>(fragmentGuid);
            const auto shader = std::make_shared<Shader>(vertexSource, fragmentSource);
            _shaders[vertexGuid + fragmentGuid] = shader;
            return shader;
        }

        return _shaders[vertexGuid + fragmentGuid];
    }

    return {};
}

std::shared_ptr<Material> Scene::GetMaterial(const std::string &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (_materials.find(guid) == _materials.end()) {
            const auto materialAsset = assetManager->LoadAssetByGuid<MaterialAsset>(guid);
            const auto shader = GetShader(materialAsset.VertexShaderGuid, materialAsset.FragmentShaderGuid);
            const auto material = std::make_shared<Material>(shader);

            const auto font = GetFont(materialAsset.Font);
            material->SetFont(font);
            material->SetBlendMode(static_cast<BlendMode>(materialAsset.BlendMode));
            material->SetCulling(materialAsset.IsCulling);

            if (!materialAsset.Image.empty()) {
                const auto sprite = GetSprite(materialAsset.Image);
                material->AddSprite(sprite);
            }

            _materials[guid] = material;
            return material;
        }
        return _materials[guid];
    }

    return {};
}

std::shared_ptr<Mesh> Scene::GetMesh(const std::string &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (_mesh.find(guid) == _mesh.end()) {
            const auto meshAsset = std::shared_ptr<Mesh>(Mesh::GenerateSpritePtr());
            _mesh[guid] = meshAsset;
            return meshAsset;
        }
        return _mesh[guid];
    }

    return {};
}

std::shared_ptr<Sprite> Scene::GetSprite(const std::string &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (_sprites.find(guid) == _sprites.end()) {
            const auto sprite = std::make_shared<Sprite>(assetManager->LoadSourceByGuid<Sprite>(guid));
            _sprites[guid] = sprite;
            return sprite;
        }
        return _sprites[guid];
    }
    return {};
}

std::shared_ptr<Font> Scene::GetFont(const std::string &guid) {
    if (guid.empty())
        return {};

    if (const auto assetManager = _assetManager.lock()) {
        if (_fonts.find(guid) == _fonts.end()) {
            auto fontFile = assetManager->LoadSourceByGuid<Font>(guid);
            const auto font = std::make_shared<Font>(std::move(fontFile));
            _fonts[guid] = font;
            return font;
        }
        return _fonts[guid];
    }
    return {};
}


std::weak_ptr<Entity> Scene::FindByTag(const std::string &tag) const {

    const auto view = _entityRegistry->view<std::shared_ptr<Entity>>();

    for (const auto &entity: view) {
        if (const auto& en = view.get<std::shared_ptr<Entity>>(entity); en->GetTag() == tag) {
            return en;
        }
    }
    return {};
}

std::weak_ptr<UiRaycastSystem> Scene::GetUiRaycast() const {
    return _uiRaycastSystem;
}
