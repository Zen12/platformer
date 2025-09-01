#pragma once
#include <unordered_map>
#include <vector>
#include "../components/entity.hpp"
#include "../components/ui_show_fps.hpp"

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

struct PrefabInstantiateData {
    std::string Id;
    glm::vec3 Position{glm::vec3(1)};
    glm::vec3 Rotation{glm::vec3(0)};
    glm::vec3 Scale{glm::vec3(1)};
};

class Scene {
private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> _shaders;
    std::unordered_map<std::string, std::shared_ptr<Material>> _materials;
    std::unordered_map<std::string, std::shared_ptr<Sprite>> _sprites;
    std::vector<std::shared_ptr<SpineData>> _spineDatas;
    std::unordered_map<std::string, std::shared_ptr<Font>> _fonts;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> _meshes;

    std::shared_ptr<RenderPipeline> _renderPipeline;
    std::weak_ptr<Window> _window;
    std::weak_ptr<AssetManager> _assetManager;
    std::weak_ptr<InputSystem> _inputSystem;

    std::shared_ptr<RectTransformRoot> _root;

    std::shared_ptr<PhysicsWorld> _physicsWorld = std::make_shared<PhysicsWorld>(b2Vec2{0.0, -10.0});

    std::vector<std::shared_ptr<Entity>> Entities;

    std::vector<std::string> _entitiesToRemove{};

public:

    std::vector<PrefabInstantiateData> PrefabRequestInstantiate{};

    Scene(
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<InputSystem> &inputSystem)
        :  _window(window), _assetManager(assetManager),_inputSystem(inputSystem)

    {
        _root = std::make_shared<RectTransformRoot>(window);
        _renderPipeline = std::make_shared<RenderPipeline>(window);
        _renderPipeline->Init();
    }

    [[nodiscard]] std::shared_ptr<RectTransformRoot> GetRoot() const;

    [[nodiscard]] std::weak_ptr<PhysicsWorld> GetPhysicsWorld() const noexcept;

    [[nodiscard]] std::weak_ptr<RenderPipeline> GetRenderPipeline() const noexcept;

    [[nodiscard]] std::weak_ptr<AssetManager> GetAssetManager() const noexcept;

    [[nodiscard]] std::weak_ptr<InputSystem> GetInputSystem() const noexcept;

    [[nodiscard]] std::weak_ptr<Window> GetWindow() const noexcept;

    std::weak_ptr<Entity> CreateEntity(const EntitySerialization &entitySerialization);


    [[nodiscard]] std::weak_ptr<Entity> GetEntityById(const std::string &id) const;

    [[nodiscard]] size_t GetEntityCount() const noexcept;

    [[nodiscard]] std::shared_ptr<Entity> GetEntityByIndex(const uint32_t& index) const noexcept;

    void RemoveEntityById(const std::string &id);

    void Update(const float &deltaTime) const;

    void RemovePendingEntities();

    void Render(const float &deltaTime) const;

    [[nodiscard]] std::shared_ptr<Shader> GetShader(const std::string &vertexGuid, const std::string &fragmentGuid);

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const std::string &guid);

    [[nodiscard]] std::shared_ptr<SpineData> LoadSpineData(const SpineAsset& asset) const;


    [[nodiscard]] std::shared_ptr<SpineData> GetSpineData([[maybe_unused]] const std::string &guid, const SpineAsset& asset);

    [[nodiscard]] std::shared_ptr<Sprite> GetSprite(const std::string &guid);

    [[nodiscard]] std::shared_ptr<Font> GetFont(const std::string &guid);

    [[nodiscard]] std::shared_ptr<Entity> GetEntity(const std::string &id) const;

    [[nodiscard]] std::weak_ptr<Entity> FindByTag(const std::string &tag) const;
};
