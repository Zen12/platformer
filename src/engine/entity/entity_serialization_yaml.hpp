#pragma once

#include "entity_serialization.hpp"
#include "../math/glm_vec_yaml.hpp"
#include "../game/health_bar/health_bar_component_serialization.hpp"
#include "../game/health_bar/health_bar_component_serialization_yaml.hpp"
#include "../ui/text/text_renderer_component_serialization.hpp"
#include "../ui/text/text_renderer_component_serialization_yaml.hpp"
#include "../ui/image/image_renderer_component_serialization.hpp"
#include "../ui/image/image_renderer_component_serialization_yaml.hpp"
#include "../ui/button/button_component_serialization.hpp"
#include "../ui/button/button_component_serialization_yaml.hpp"
#include "../ui/on_click_scene_loader/on_click_scene_loader_serialization.hpp"
#include "../ui/on_click_scene_loader/on_click_scene_loader_serialization_yaml.hpp"
#include "../ui/button_effect/ui_button_effect_serialization.hpp"
#include "../ui/button_effect/ui_button_effect_serialization_yaml.hpp"
#include "../ui/show_fps/show_fps_component_serialization.hpp"
#include "../ui/show_fps/show_fps_component_serialization_yaml.hpp"
#include "../ui/rect_transform/rect_transform_component_serialization.hpp"
#include "../ui/rect_transform/rect_transform_component_serialization_yaml.hpp"
#include "../renderer/transform/transform_component_serialization.hpp"
#include "../renderer/transform/transform_component_serialization_yaml.hpp"
#include "../renderer/sprite/sprite_renderer_component_serialization.hpp"
#include "../renderer/sprite/sprite_renderer_component_serialization_yaml.hpp"
#include "../renderer/spine/spine_renderer_component_serialization.hpp"
#include "../renderer/spine/spine_renderer_component_serialization_yaml.hpp"
#include "../renderer/light_2d/light_2d_component_serialization.hpp"
#include "../renderer/light_2d/light_2d_component_serialization_yaml.hpp"
#include "../renderer/mesh/mesh_renderer_component_serialization.hpp"
#include "../renderer/mesh/mesh_renderer_component_serialization_yaml.hpp"
#include "../renderer/particles/particle_emitter_component_serialization.hpp"
#include "../renderer/particles/particle_emitter_component_serialization_yaml.hpp"
#include "../physics/spine_collider/spine_collider_component_serialization.hpp"
#include "../physics/spine_collider/spine_collider_component_serialization_yaml.hpp"
#include "../game/rect_transform_follower/rect_transform_follower_component_serialization.hpp"
#include "../game/rect_transform_follower/rect_transform_follower_component_serialization_yaml.hpp"
#include "../physics/collider/box_collider2d_component_serialization.hpp"
#include "../physics/collider/box_collider2d_component_serialization_yaml.hpp"
#include "../physics/rigidbody/rigidbody2d_component_serialization.hpp"
#include "../physics/rigidbody/rigidbody2d_component_serialization_yaml.hpp"
#include "../renderer/camera/camera_component_serialization.hpp"
#include "../renderer/camera/camera_component_serialization_yaml.hpp"
#include "../game/destroy_with_creator/destroy_with_creator_component_serialization.hpp"
#include "../game/destroy_with_creator/destroy_with_creator_component_serialization_yaml.hpp"
#include "../game/idle_character/idle_character_component_serialization.hpp"
#include "../game/idle_character/idle_character_component_serialization_yaml.hpp"
#include "../game/game_state/game_state_component_serialization.hpp"
#include "../game/game_state/game_state_component_serialization_yaml.hpp"
#include "../game/team/team_component_serialization.hpp"
#include "../game/team/team_component_serialization_yaml.hpp"
#include "../game/prefab_spawner/prefab_spawner_component_serialization.hpp"
#include "../game/prefab_spawner/prefab_spawner_component_serialization_yaml.hpp"
#include "../game/grid_prefab_spawner/grid_prefab_spawner_component_serialization.hpp"
#include "../game/grid_prefab_spawner/grid_prefab_spawner_component_serialization_yaml.hpp"
#include "../game/grid/grid_component_serialization.hpp"
#include "../game/grid/grid_component_serialization_yaml.hpp"
#include "../nav_mesh/astar_finder_component_serialization.hpp"
#include "../nav_mesh/astar_finder_component_serialization_yaml.hpp"
#include "../nav_mesh/path_mover_component_serialization.hpp"
#include "../nav_mesh/path_mover_component_serialization_yaml.hpp"
#include "../game/health/health_component_serialization.hpp"
#include "../game/health/health_component_serialization_yaml.hpp"
#include "../game/character_controller/character_controller_component_serialization.hpp"
#include "../game/character_controller/character_controller_component_serialization_yaml.hpp"
#include "../game/character_controller/ai_controller/ai_controller_component_serialization.hpp"
#include "../game/character_controller/ai_controller/ai_controller_component_serialization_yaml.hpp"
#include "../game/character_controller/character_animation/character_animation_component_serialization.hpp"
#include "../game/character_controller/character_animation/character_animation_component_serialization_yaml.hpp"
#include "../game/character_controller/movement/character_movement_component_serialization.hpp"
#include "../game/character_controller/movement/character_movement_component_serialization_yaml.hpp"
#include "../game/character_controller/shoot/shoot_component_serialization.hpp"
#include "../game/character_controller/shoot/shoot_component_serialization_yaml.hpp"

namespace YAML {
    template <>
    struct convert<EntitySerialization>
    {
        template<typename T>
        static std::unique_ptr<T> Parse(const YAML::Node &data) {
            static_assert(std::is_base_of_v<ComponentSerialization, T>,
                      "T must derive from ComponentSerialization");

            const auto comp = data.as<T>();
            return std::make_unique<T>(comp);
        }

        static bool decode(const Node &node, EntitySerialization &rhs)
        {
            if (node["creator"]) {
                rhs.Creator = node["creator"].as<std::string>();
            }
            if (node["tag"]) {
                rhs.Tag = node["tag"].as<std::string>();
            }
            if (node["guid"]) {
                rhs.Guid = node["guid"].as<std::string>();
            }
            if (node["layer"]) {
                rhs.Layer = node["layer"].as<int>();
            }

            if (node["components"]) {
                for (const auto& comp : node["components"]) {
                    rhs.Components.push_back(createComponentFromYAML(comp));
                }
            }

            return true;
        }

        static std::unique_ptr<ComponentSerialization>
        createComponentFromYAML(const YAML::Node& node) {

            const auto type = node["type"].as<std::string>();

            using Factory = std::function<std::unique_ptr<ComponentSerialization>(const YAML::Node&)>;
            static const std::unordered_map<std::string, Factory> factories = {
                { "camera",                [](const YAML::Node& n){ return Parse<CameraComponentSerialization>(n); } },
                { "transform",             [](const YAML::Node& n){ return Parse<TransformComponentSerialization>(n); } },
                { "rect_transform",        [](const YAML::Node& n){ return Parse<RectTransformComponentSerialization>(n); } },
                { "ui_image",              [](const YAML::Node& n){ return Parse<UiImageComponentSerialization>(n); } },
                { "ui_button",             [](const YAML::Node& n){ return Parse<UiButtonComponentSerialization>(n); } },
                { "ui_text",               [](const YAML::Node& n){ return Parse<UiTextComponentSerialization>(n); } },
                { "sprite_renderer",       [](const YAML::Node& n){ return Parse<SpriteRenderComponentSerialization>(n); } },
                { "box_collider",          [](const YAML::Node& n){ return Parse<Box2dColliderSerialization>(n); } },
                { "rigidbody2d",           [](const YAML::Node& n){ return Parse<Rigidbody2dSerialization>(n); } },
                { "light_2d",              [](const YAML::Node& n){ return Parse<Light2dComponentSerialization>(n); } },
                { "mesh_renderer",         [](const YAML::Node& n){ return Parse<MeshRendererComponentSerialization>(n); } },
                { "spine_renderer",        [](const YAML::Node& n){ return Parse<SpineRenderComponentSerialization>(n); } },
                { "character_controller",  [](const YAML::Node& n){ return Parse<CharacterControllerComponentSerialization>(n); } },
                { "character_movement",    [](const YAML::Node& n){ return Parse<CharacterMovementComponentSerialization>(n); } },
                { "character_animation",   [](const YAML::Node& n){ return Parse<CharacterAnimationComponentSerialization>(n); } },
                { "shoot",                 [](const YAML::Node& n){ return Parse<ShootComponentSerialization>(n); } },
                { "ai_controller",         [](const YAML::Node& n){ return Parse<AiControllerComponentSerialization>(n); } },
                { "ui_show_fps",           [](const YAML::Node& n){ return Parse<ShowFpsComponentSerialization>(n); } },
                { "health_component",      [](const YAML::Node& n){ return Parse<HealthComponentSerialization>(n); } },
                { "prefab_spawner",        [](const YAML::Node& n){ return Parse<PrefabSpawnerSerialization>(n); } },
                { "grid_prefab_spawner",   [](const YAML::Node& n){ return Parse<GridPrefabSpawnerSerialization>(n); } },
                { "grid",                  [](const YAML::Node& n){ return Parse<GridSerialization>(n); } },
                { "astar_path_finder",     [](const YAML::Node& n){ return Parse<PathFinderSerialization>(n); } },
                { "particle_emitter",      [](const YAML::Node& n){ return Parse<ParticleEmitterSerialization>(n); } },
                { "spine_collider",        [](const YAML::Node& n){ return Parse<SpineColliderSerialization>(n); } },
                { "health_bar",            [](const YAML::Node& n){ return Parse<HealthBarComponentSerialization>(n); } },
                { "rect_transform_follower",[](const YAML::Node& n){ return Parse<RectTransformFollowerSerialization>(n); } },
                { "destroy_with_creator",  [](const YAML::Node& n){ return Parse<DestroyWithCreatorComponentSerialization>(n); } },
                { "idle_character",        [](const YAML::Node& n){ return Parse<IdleCharacterSerialization>(n); } },
                { "on_click_scene_loader", [](const YAML::Node& n){ return Parse<OnClickSceneLoaderSerialization>(n); } },
                { "path_mover",            [](const YAML::Node& n){ return Parse<PathMoverComponentSerialization>(n); } },
                { "game_state",            [](const YAML::Node& n){ return Parse<GameStateData>(n); } },
                { "team",                  [](const YAML::Node& n){ return Parse<TeamSerialization>(n); } },
                { "ui_button_effect",      [](const YAML::Node& n){ return Parse<UiButtonEffectSerialization>(n); } },
            };

            if (const auto it = factories.find(type); it != factories.end()) {
                return it->second(node);
            }

            throw std::runtime_error("Unknown component type: " + type);
        }

    };
}
