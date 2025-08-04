#pragma once

#include "../components/entity.hpp"
#include "../components/transforms/rect_transform_root.hpp"
#include "../render/shader.hpp"
#include "../render/material.hpp"
#include "../render/spine/spine_data.hpp"
#include "../render_pipeline/render_pipeline.hpp"
#include "../asset/asset_manager.hpp"
#include "../physics/physics_world.hpp"
#include "../components/light_2d.hpp"
#include "../components/spine_renderer.hpp"
#include "../components/physics/box_collider2d_component.hpp"
#include "../components/physics/rigidbody2d_component.hpp"
#include "../debug/debug.hpp"


class SceneManager {

private:
    std::vector<std::shared_ptr<Entity>> _entities;
    std::unordered_map<std::string, std::shared_ptr<Shader>> _shaders;
    std::unordered_map<std::string, std::shared_ptr<Material>> _materials;
    std::unordered_map<std::string, std::shared_ptr<Sprite>> _sprites;
    std::unordered_map<std::string, std::shared_ptr<SpineData>> _spineDatas;
    std::unordered_map<std::string, std::shared_ptr<Font>> _fonts;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> _meshes;

    std::weak_ptr<RenderPipeline> _renderPipeline;
    std::weak_ptr<Window> _window;
    std::weak_ptr<AssetManager> _assetManager;

    std::shared_ptr<RectTransformRoot> _root;

    std::shared_ptr<PhysicsWorld> _physicsWorld = std::make_shared<PhysicsWorld>(b2Vec2{0.0, -10.0});


    public:
    SceneManager(
        const std::weak_ptr<RenderPipeline> &render,
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<AssetManager> &assetManager)
        : _renderPipeline(render), _window(window), _assetManager(assetManager) {
        _root = std::make_shared<RectTransformRoot>(window);
    }

    void LoadScene(const SceneAsset& serialization);
    void UnLoadAll();

    std::shared_ptr<PhysicsWorld> GetPhysicsWorld() {
        return _physicsWorld;
    }

    [[nodiscard]] std::weak_ptr<Entity> GetEntityById(const std::string& id) const{
        for (const auto &entity: _entities) {
            if (entity->GetId() == id) {
                return entity;
            }
        }
        return {};
    }

    void Update() const {
        for (const auto &entity: _entities) {
            entity->Update();
        }
    }

private:
    [[nodiscard]] std::shared_ptr<Shader> GetShader(const std::string &vertexGuid, const std::string &fragmentGuid);

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const std::string& guid);

    std::shared_ptr<SpineData> LoadSpineData(const SpineAsset &asset) const;

    [[nodiscard]] std::shared_ptr<Sprite> GetSprite(const std::string& guid);

    [[nodiscard]] std::shared_ptr<Font> GetFont(const std::string& guid);

    [[nodiscard]] std::shared_ptr<Entity> GetEntity(const std::string& id) const;

    [[nodiscard]] std::shared_ptr<SpineData> GetSpineData(const std::string &guid, const SpineAsset& asset);



};