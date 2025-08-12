#pragma once

#include "scene.hpp"
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
#include "../system/input_system.hpp"


class SceneManager {

    std::shared_ptr<Scene> _scene;

public:
    SceneManager(
        const std::weak_ptr<RenderPipeline> &render,
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<InputSystem> &inputSystem)
        : _scene(std::make_shared<Scene>(render, window, assetManager, inputSystem))
        {
    }

    void LoadScene(const SceneAsset& serialization) const;
    void UnLoadAll() const;

    [[nodiscard]] std::weak_ptr<PhysicsWorld> GetPhysicsWorld() const {
        return _scene->GetPhysicsWorld();;
    }

    [[nodiscard]] std::weak_ptr<Entity> GetEntityById(const std::string& id) const;

    void Update(const float& deltaTime) const;

private:
    [[nodiscard]] std::shared_ptr<Shader> GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) const;

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const std::string& guid) const;

    [[nodiscard]] std::shared_ptr<SpineData> LoadSpineData(const SpineAsset &asset) const;

    [[nodiscard]] std::shared_ptr<Sprite> GetSprite(const std::string& guid) const;

    [[nodiscard]] std::shared_ptr<Font> GetFont(const std::string& guid) const;

    [[nodiscard]] std::shared_ptr<Entity> GetEntity(const std::string& id) const;

    [[nodiscard]] std::shared_ptr<SpineData> GetSpineData(const std::string &guid, const SpineAsset& asset) const;

};
