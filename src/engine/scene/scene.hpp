#pragma once
#include <unordered_map>
#include <vector>
#include "../components/entity.hpp"

#include "../components/transforms/rect_transform_root.hpp"
#include "../render/shader.hpp"
#include "../render/material.hpp"
#include "../render/spine/spine_data.hpp"
#include "../render_pipeline/render_pipeline.hpp"
#include "../asset/asset_manager.hpp"
#include "../physics/physics_world.hpp"
#include "../components/light_2d.hpp"
#include "../gameplay/character_controller.hpp"
#include "../components/spine_renderer.hpp"
#include "../components/physics/box_collider2d_component.hpp"
#include "../components/physics/rigidbody2d_component.hpp"
#include "../debug/debug.hpp"
#include "../system/input_system.hpp"

class Scene {
private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> _shaders;
    std::unordered_map<std::string, std::shared_ptr<Material>> _materials;
    std::unordered_map<std::string, std::shared_ptr<Sprite>> _sprites;
    std::unordered_map<std::string, std::shared_ptr<SpineData>> _spineDatas;
    std::unordered_map<std::string, std::shared_ptr<Font>> _fonts;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> _meshes;

    std::weak_ptr<RenderPipeline> _renderPipeline;
    std::weak_ptr<Window> _window;
    std::weak_ptr<AssetManager> _assetManager;
    std::weak_ptr<InputSystem> _inputSystem;

    std::shared_ptr<RectTransformRoot> _root;

    std::shared_ptr<PhysicsWorld> _physicsWorld = std::make_shared<PhysicsWorld>(b2Vec2{0.0, -10.0});

    std::vector<std::shared_ptr<Entity>> Entities;

public:

    Scene(
        const std::weak_ptr<RenderPipeline> &render,
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<InputSystem> &inputSystem)
        : _renderPipeline(render), _window(window), _assetManager(assetManager),_inputSystem(inputSystem)

    {
        _root = std::make_shared<RectTransformRoot>(window);
    }

    [[nodiscard]] std::shared_ptr<RectTransformRoot> GetRoot() const {
        return _root;
    }

    [[nodiscard]] std::weak_ptr<PhysicsWorld> GetPhysicsWorld() const noexcept {
        return _physicsWorld;
    }

    [[nodiscard]] std::weak_ptr<RenderPipeline> GetRenderPipeline() const noexcept {
        return _renderPipeline;
    }

    [[nodiscard]] std::weak_ptr<AssetManager> GetAssetManager() const noexcept {
        return _assetManager;
    }

    [[nodiscard]] std::weak_ptr<InputSystem> GetInputSystem() const noexcept {
        return _inputSystem;
    }

    [[nodiscard]] std::weak_ptr<Window> GetWindow() const noexcept {
        return _window;
    }

    void AddEntity(const std::shared_ptr<Entity> &entity) {
        Entities.push_back(entity);
    }

    void UnLoadAll() {
        Entities.clear();
        _shaders.clear();
        _materials.clear();
        _sprites.clear();
        _spineDatas.clear();
        _fonts.clear();
        _meshes.clear();
    }


    [[nodiscard]] std::weak_ptr<Entity> GetEntityById(const std::string &id) const {
        for (const auto &entity: Entities) {
            if (entity->GetId() == id) {
                return entity;
            }
        }
        return {};
    }

    [[nodiscard]] size_t GetEntityCount() const noexcept {
        return Entities.size();
    }

    [[nodiscard]] std::shared_ptr<Entity> GetEntityByIndex(const uint32_t& index) const noexcept {
        if (index >= Entities.size()) {
            return {};
        }
        return Entities[index];
    }

    void Update(const float &deltaTime) const {
        for (const auto &entity: Entities) {
            entity->Update(deltaTime);
        }
    }

    [[nodiscard]] std::shared_ptr<Shader> GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) {
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

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const std::string &guid) {
        if (const auto assetManager = _assetManager.lock()) {
            if (_materials.find(guid) == _materials.end()) {
                const auto materialAsset = assetManager->LoadAssetByGuid<MaterialAsset>(guid);
                const auto shader = GetShader(materialAsset.VertexShaderGuid, materialAsset.FragmentShaderGuid);
                const auto material = std::make_shared<Material>(shader);

                const auto font = GetFont(materialAsset.Font);
                material->SetFont(font);

                _materials[guid] = material;
                return material;
            }
            return _materials[guid];
        }

        return {};
    }

    [[nodiscard]] std::shared_ptr<SpineData> LoadSpineData(const SpineAsset& asset) const {
        if (auto assetManager = _assetManager.lock()) {
            // Load atlas (Spine owns this pointer)
            auto atlas = assetManager->LoadSourceByGuid<spine::Atlas*>(asset.atlas);

            spine::SkeletonBinary binary(atlas);

            // Load SkeletonData (Spine allocates it with new)
            spine::SkeletonData* rawSkeletonData = binary.readSkeletonDataFile(
                assetManager->GetPathFromGuid(asset.skeleton).c_str()
            );

            // Wrap in smart pointer so it's freed automatically
            auto skeletonData = std::shared_ptr<spine::SkeletonData>(rawSkeletonData);

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
                animationState
            );
        }

        return {};
    }


    [[nodiscard]] std::shared_ptr<SpineData> GetSpineData(const std::string &guid, const SpineAsset& asset) {

        if (const auto assetManager = _assetManager.lock()) {
            if (_spineDatas.find(guid) == _spineDatas.end()) {
                const auto spineData = LoadSpineData(asset);
                _spineDatas[guid] = spineData;
                return spineData;
            }
            return _spineDatas[guid];
        }
        return {};
    }

    [[nodiscard]] std::shared_ptr<Sprite> GetSprite(const std::string &guid) {
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

    [[nodiscard]] std::shared_ptr<Font> GetFont(const std::string &guid) {
        if (guid.empty())
            return {};

        if (const auto assetManager = _assetManager.lock()) {
            if (_fonts.find(guid) == _fonts.end()) {
                const auto fontFile = assetManager->LoadSourceByGuid<Font>(guid);
                std::shared_ptr<Font> font = std::make_shared<Font>(fontFile);
                _fonts[guid] = font;
                return font;
            }
            return _fonts[guid];
        }
        return {};
    }

    [[nodiscard]] std::shared_ptr<Entity> GetEntity(const std::string &id) const {
        for (const auto & entity: Entities) {
            if (entity->GetId() == id)
                return entity;
        }
        return {};
    }

    [[nodiscard]] std::weak_ptr<Entity> FindByTag(const std::string &tag) const {
        for (const auto & entity: Entities) {
            if (entity->GetTag() == tag)
                return entity;
        }
        return {};
    }
};
