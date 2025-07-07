#pragma once

#include "../components/entity.hpp"
#include "../components/transforms/rect_transform_root.hpp"
#include "../render/shader.hpp"
#include "../render/material.hpp"
#include "../render_pipeline/render_pipeline.hpp"
#include "../asset/asset_deserializer.hpp"
#include "../asset/asset_manager.hpp"
#include "../components/physics/box_collider2d_component.hpp"
#include "../physics/physics_world.hpp"



class SceneManager {

private:
    std::vector<std::shared_ptr<Entity>> _entities;
    std::vector<std::shared_ptr<Shader>> _shaders;
    std::vector<std::shared_ptr<Material>> _materials;
    std::vector<std::shared_ptr<Sprite>> _sprites;
    std::vector<std::shared_ptr<Font>> _fonts;

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
};