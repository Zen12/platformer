#pragma once

#include "scene.hpp"
#include "../components/transforms/rect_transform_root.hpp"
#include "../render/shader.hpp"
#include "../render/material.hpp"
#include "../render/spine/spine_data.hpp"
#include "../render_pipeline/render_pipeline.hpp"
#include "../asset/asset_manager.hpp"
#include "../physics/physics_world.hpp"
#include "../renderer/light_2d/light_2d_component.hpp"
#include "../renderer/spine/spine_renderer_component.hpp"
#include "../physics/collider/box_collider2d_component.hpp"
#include "../physics/rigidbody/rigidbody2d_component.hpp"
#include "../debug/debug.hpp"
#include "../system/input_system.hpp"
#include "../system/guid_generator.hpp"

#include "../asset/factories/component_factory.hpp"
#include "../gameplay/health_bar/health_bar_component_factory.hpp"
#include "../ui/text/text_renderer_component_factory.hpp"
#include "../ui/image/image_renderer_component_factory.hpp"
#include "../ui/button/button_component_factory.hpp"
#include "../ui/on_click_scene_loader/on_click_scene_loader_factory.hpp"
#include "../ui/button_effect/ui_button_effect_factory.hpp"
#include "../ui/show_fps/show_fps_component_factory.hpp"
#include "../ui/rect_transform/rect_transform_component_factory.hpp"
#include "../renderer/transform/transform_component_factory.hpp"
#include "../renderer/sprite/sprite_renderer_component_factory.hpp"
#include "../renderer/spine/spine_renderer_component_factory.hpp"
#include "../renderer/light_2d/light_2d_component_factory.hpp"
#include "../renderer/mesh/mesh_renderer_component_factory.hpp"
#include "../renderer/particles/particle_emitter_component_factory.hpp"
#include "../physics/spine_collider/spine_collider_component_factory.hpp"
#include "../game/rect_transform_follower/rect_transform_follower_component_factory.hpp"
#include "../physics/collider/box_collider2d_component_factory.hpp"
#include "../physics/rigidbody/rigidbody2d_component_factory.hpp"
#include "../camera/camera_component_factory.hpp"
#include "../game/destroy_with_creator/destroy_with_creator_component_factory.hpp"
#include "../game/idle_character/idle_character_component_factory.hpp"
#include "../game/game_state/game_state_component_factory.hpp"
#include "../game/team/team_component_factory.hpp"
#include "../game/prefab_spawner/prefab_spawner_component_factory.hpp"
#include "../game/grid_prefab_spawner/grid_prefab_spawner_component_factory.hpp"
#include "../game/grid/grid_component_factory.hpp"
#include "../nav_mesh/astar_finder_component_factory.hpp"
#include "../nav_mesh/path_mover_component_factory.hpp"
#include "../game/health/health_component_factory.hpp"
#include "../game/character_controller/character_controller_component_factory.hpp"
#include "../game/ai_controller/ai_controller_component_factory.hpp"



class SceneManager {

    std::shared_ptr<Scene> _scene;
    std::weak_ptr<AssetManager> _assetManager;
    std::weak_ptr<Window> _window;
    std::weak_ptr<InputSystem> _inputSystem;

public:
    SceneManager(
        const std::weak_ptr<Window> &window,
        const std::weak_ptr<AssetManager> &assetManager,
        const std::weak_ptr<InputSystem> &inputSystem) {
        _assetManager = assetManager;
        _window = window;
        _inputSystem = inputSystem;
    }

    void LoadScene(const SceneAsset& serialization);
    void LoadEntities(const std::vector<EntitySerialization> &serialization) const;

    [[nodiscard]] std::weak_ptr<PhysicsWorld> GetPhysicsWorld() const {
        return _scene->GetPhysicsWorld();;
    }

    [[nodiscard]] std::weak_ptr<Entity> GetEntityById(const std::string& id) const;

    void CreateRequestedPrefabs() const;

    void Update(const float& deltaTime);

    void Render(const float &deltaTime) const;

    [[nodiscard]] bool IsRequestToLoadScene() const;

    void LoadRequestedScene();


private:
    [[nodiscard]] std::shared_ptr<Shader> GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) const;

    [[nodiscard]] std::shared_ptr<Material> GetMaterial(const std::string& guid) const;

    [[nodiscard]] std::shared_ptr<SpineData> LoadSpineData(const SpineAsset &asset) const;

    [[nodiscard]] std::shared_ptr<Sprite> GetSprite(const std::string& guid) const;

    [[nodiscard]] std::shared_ptr<Font> GetFont(const std::string& guid) const;

    [[nodiscard]] std::shared_ptr<Entity> GetEntity(const std::string& id) const;

    [[nodiscard]] std::shared_ptr<SpineData> GetSpineData(const std::string &guid, const SpineAsset& asset) const;

    template <typename TSerialization, typename TFactory>
    void AddComponent(std::weak_ptr<Entity> e, TSerialization *serialization) const;

    template<typename TComponent, typename TFactory>
    bool TryToAddComponent(ComponentSerialization* component, std::weak_ptr<Entity> e) const;

    template<class SerializationT, class FactoryT, class ... Rest>
    bool TryToAddComponents(ComponentSerialization *comp, std::weak_ptr<Entity> e) const;
};


