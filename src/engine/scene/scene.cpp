#include "scene.hpp"
#include "../renderer/material/material_asset.hpp"
#include "../renderer/material/material_asset_yaml.hpp"

std::shared_ptr<RectTransformRoot> Scene::GetRoot() const {
    return _root;
}

std::weak_ptr<PhysicsWorld> Scene::GetPhysicsWorld() const noexcept {
    return _physicsWorld;
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

    Entities.push_back(newEntity);
    return newEntity;
}

std::weak_ptr<Entity> Scene::GetEntityById(const std::string &id) const {
    for (const auto &entity: Entities) {
        if (entity->GetId() == id) {
            return entity;
        }
    }
    return {};
}

size_t Scene::GetEntityCount() const noexcept {
    return Entities.size();
}

std::shared_ptr<Entity> Scene::GetEntityByIndex(const uint32_t &index) const noexcept {
    if (index >= Entities.size()) {
        return {};
    }
    return Entities[index];
}

void Scene::RemoveEntityById(const std::string &id) {
    _entitiesToRemove.push_back(id);
}

void Scene::Update(const float &deltaTime) const {
    for (const auto &entity: Entities) {
        entity->Update(deltaTime);
    }
    _uiRaycastSystem->UpdateState();
    _physicsWorld->UpdateRigidBodies();
}

void Scene::RemovePendingEntities() {
    for (const auto & id: _entitiesToRemove) {

        const auto it = std::find_if(Entities.begin(), Entities.end(),
                                     [id](const std::shared_ptr<Entity> &entity) {
                                         return entity->GetId() == id;
                                     });

        if (it != Entities.end()) {
            _physicsWorld->RemoveRigidBody(it->get()->GetComponent<Rigidbody2dComponent>());
            Entities.erase(it);
        }
    }
}

void Scene::Render(const float &deltaTime) const {

    _renderPipeline->ClearFrame();
    _renderPipeline->RenderSprites(deltaTime);
    _renderPipeline->RenderParticles(deltaTime);
    _renderPipeline->RenderMeshes(deltaTime);

    _renderPipeline->RenderDebugLines();
    _renderPipeline->RenderUI(deltaTime);
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

std::shared_ptr<SpineData> Scene::LoadSpineData(const SpineAsset &asset) const {
    if (auto assetManager = _assetManager.lock()) {
        // Load atlas (Spine owns this pointer)
        auto atlas = assetManager->LoadSourceByGuid<spine::Atlas*>(asset.atlas);

#ifndef NDEBUG
        if (atlas && atlas->getPages().size() > 0) {
            std::cout << "Atlas is valid. Pages: " << atlas->getPages().size() << std::endl;
        } else {
            std::cerr << "Atlas failed to load or is empty!" << std::endl;
        }
#endif
        spine::SkeletonBinary binary(atlas);

        std::ifstream file(assetManager->GetPathFromGuid(asset.skeleton), std::ios::binary);

        std::string buffer((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

        const unsigned char* skelData = reinterpret_cast<const unsigned char*>(buffer.data());
        size_t skelSize = buffer.size();

        spine::SkeletonData* skeletonDataRaw = binary.readSkeletonData(skelData, skelSize);

        if (!skeletonDataRaw) {
            std::cerr << "Failed to load skeleton from memory!" << std::endl;
            return nullptr;
        }
        // Wrap in smart pointer so it's freed automatically
        auto skeletonData = std::shared_ptr<spine::SkeletonData>(skeletonDataRaw);

        // Create AnimationStateData and wrap in smart pointer
        auto stateData = std::make_shared<spine::AnimationStateData>(skeletonData.get());

        // Create Skeleton and AnimationState
        auto skeleton = std::make_shared<spine::Skeleton>(skeletonData.get());
        auto animationState = std::make_shared<spine::AnimationState>(stateData.get());

        // Store all in SpineData so nothing is destroyed prematurely
        return std::make_shared<SpineData>(
            skeletonData,
            stateData,
            skeleton,
            animationState,
            asset.moveAnimationName,
            asset.jumpAnimationName,
            asset.hitAnimationName,
            asset.idleAnimationName
        );
    }

    return {};
}

std::shared_ptr<SpineData> Scene::GetSpineData([[maybe_unused]] const std::string &guid, const SpineAsset &asset) {

    const auto spine = LoadSpineData(asset);
    _spineDatas.push_back(spine);
    return spine;
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
            const auto fontFile = assetManager->LoadSourceByGuid<Font>(guid);
            const auto font = std::make_shared<Font>(fontFile);
            _fonts[guid] = font;
            return font;
        }
        return _fonts[guid];
    }
    return {};
}

std::shared_ptr<Entity> Scene::GetEntity(const std::string &id) const {
    for (const auto & entity: Entities) {
        if (entity->GetId() == id)
            return entity;
    }
    return {};
}

std::weak_ptr<Entity> Scene::FindByTag(const std::string &tag) const {
    for (const auto & entity: Entities) {
        if (entity->GetTag() == tag)
            return entity;
    }
    return {};
}

std::weak_ptr<UiRaycastSystem> Scene::GetUiRaycast() const {
    return _uiRaycastSystem;
}
