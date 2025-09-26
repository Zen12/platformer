#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <memory>
#include "glm/vec4.hpp"

#include "../serialization/serialization_component.hpp"
#include "../serialization/serialization_asset.hpp"
#include "../../gameplay/health_bar/health_bar_component_serialization.hpp"
#include "../../gameplay/health_bar/health_bar_component_serialization_yaml.hpp"

namespace YAML
{

    template <>
    struct convert<glm::vec2>
    {
        static bool decode(const Node &node, glm::vec2 &rhs)
        {
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template <>
    struct convert<glm::vec3>
    {
        static bool decode(const Node &node, glm::vec3 &rhs)
        {
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template <>
    struct convert<glm::vec4>
    {
        static bool decode(const Node &node, glm::vec4 &rhs)
        {
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template <>
    struct convert<UiTextComponentSerialization>
    {
        static bool decode(const Node &node, UiTextComponentSerialization &rhs)
        {
            rhs.MaterialGUID = node["material"].as<std::string>();
            rhs.Text = node["text"].as<std::string>();
            rhs.Color = node["color"].as<glm::vec3>();
            rhs.FontSize = node["font_size"].as<float>();
            return true;
        }
    };

    template <>
    struct convert<CharacterControllerComponentSerialization>
    {
        static bool decode(const Node &node, CharacterControllerComponentSerialization &rhs)
        {
            rhs.MaxMovementSpeed = node["max_movement_speed"].as<float>();
            rhs.AccelerationSpeed = node["acceleration_speed"].as<float>();
            rhs.DecelerationSpeed = node["deceleration_speed"].as<float>();
            rhs.JumpHeigh = node["jump_heigh"].as<float>();
            rhs.JumpDuration = node["jump_duration"].as<float>();
            rhs.JumpDownMultiplier = node["jump_down_multiplier"].as<float>();
            rhs.AirControl = node["air_control"].as<float>();
            rhs.Damage = node["damage"].as<float>();
            return true;
        }
    };

    template <>
    struct convert<AiControllerComponentSerialization>
    {
        static bool decode(const Node &node, AiControllerComponentSerialization &rhs)
        {
            rhs.MaxMovementSpeed = node["max_movement_speed"].as<float>();
            rhs.AccelerationSpeed = node["acceleration_speed"].as<float>();
            rhs.DecelerationSpeed = node["deceleration_speed"].as<float>();
            rhs.JumpHeigh = node["jump_heigh"].as<float>();
            rhs.JumpDuration = node["jump_duration"].as<float>();
            rhs.JumpDownMultiplier = node["jump_down_multiplier"].as<float>();
            rhs.AirControl = node["air_control"].as<float>();
            rhs.Damage = node["damage"].as<float>();
            rhs.AiTargetTransformTag = node["ai_target_transform_tag"].as<std::string>();
            rhs.PathFinderTag = node["path_finder_tag"].as<std::string>();
            rhs.GridTag = node["grid_tag"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<HealthComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] HealthComponentSerialization &rhs)
        {
            rhs.Health = node["health"].as<float>();
            return true;
        }
    };

    template <>
    struct convert<ShowFpsComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] ShowFpsComponentSerialization &rhs)
        {
            return true;
        }
    };

    template <>
    struct convert<CameraComponentSerialization>
    {
        static bool decode(const Node &node, CameraComponentSerialization &rhs)
        {
            rhs.aspectPower = node["aspectPower"].as<float>();
            rhs.isPerspective = node["isPerspective"].as<bool>();
            return true;
        }
    };

    template <>
    struct convert<TransformComponentSerialization>
    {
        static bool decode(const Node &node, TransformComponentSerialization &rhs)
        {
            rhs.position = node["position"].as<glm::vec3>();
            rhs.rotation = node["rotation"].as<glm::vec3>();
            rhs.scale = node["scale"].as<glm::vec3>();
            return true;
        }
    };

    template <>
    struct convert<SpineRenderComponentSerialization>
    {
        static bool decode(const Node &node, SpineRenderComponentSerialization &rhs)
        {
            rhs.SpineGuid = node["spine_data"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<MeshRendererComponentSerialization>
    {
        static bool decode(const Node &node, MeshRendererComponentSerialization &rhs)
        {
            rhs.MaterialGuid = node["material"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<Light2dComponentSerialization>
    {
        static bool decode(const Node &node, Light2dComponentSerialization &rhs) {

            rhs.CenterTransform = node["center"].as<std::string>();
            rhs.Offset = node["offset"].as<glm::vec3>();
            rhs.Color = node["color"].as<glm::vec3>();
            rhs.Radius = node["radius"].as<float>();
            rhs.Segments = node["segments"].as<int>();
            rhs.MaxAngle = node["max_angle"].as<float>();
            rhs.StartAngle = node["start_angle"].as<float>();

            return true;
        }
    };

    template <>
    struct convert<PathMoverComponentSerialization>
    {
        static bool decode(const Node &node, PathMoverComponentSerialization &rhs) {

            rhs.Positions = std::vector<glm::vec3>();

            for (const auto &path : node["positions"]) {
                rhs.Positions.push_back(path.as<glm::vec3>());
            }

            rhs.Speed = node["speed"].as<float>();

            return true;
        }
    };


    template <>
    struct convert<SpriteRenderComponentSerialization>
    {
        static bool decode(const Node &node, SpriteRenderComponentSerialization &rhs) {
            rhs.MaterialGuid = node["material"].as<std::string>();
            rhs.SpriteGuid = node["image"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<UiImageComponentSerialization>
    {
        static bool decode(const Node &node, UiImageComponentSerialization &rhs) {
            rhs.MaterialGuid = node["material"].as<std::string>();
            rhs.SpriteGuid = node["image"].as<std::string>();
            if (node["fill_amount"])
                rhs.FillAmount = node["fill_amount"].as<float>();
            return true;
        }
    };

    template <>
    struct convert<RectTransformComponentSerialization>
    {
        static void decodeLayout(const std::string& key, const Node &node, RectTransformComponentSerialization &rhs) {
            auto x = 0.0f;
            auto y = 0.0f;
            auto value =  0.0f;
            for (const auto &item : node[key]) {
                x = item["x"] ? item["x"].as<float>() : x;
                y = item["y"] ? item["y"].as<float>() : y;
                value = item["value"] ? item["value"].as<float>() : value;
            }

            rhs.Layouts.emplace_back(key, value, x, y);
        }

        static bool decode(const Node &node, RectTransformComponentSerialization &rhs) {

            if (const auto nodeValue = node["pixel_x"]) {
                decodeLayout("pixel_x", node, rhs);
            }

            if (const auto nodeValue = node["pixel_y"]) {
                decodeLayout("pixel_y", node, rhs);
            }

            if (const auto nodeValue = node["center_x"]) {
                decodeLayout("center_x", node, rhs);
            }
            if (const auto nodeValue = node["center_y"]) {
                decodeLayout("center_y", node, rhs);
            }
            if (const auto nodeValue = node["pixel_width"]) {
                decodeLayout("pixel_width", node, rhs);
            }
            if (const auto nodeValue = node["pixel_height"]) {
                decodeLayout("pixel_height", node, rhs);
            }
            if (const auto nodeValue = node["pivot"]) {
                decodeLayout("pivot", node, rhs);
            }

            return true;
        }
    };

    template <>
    struct convert<SpriteComponentSerialization>
    {
        static bool decode(const Node &node, SpriteComponentSerialization &rhs) {
            rhs.Path = node["path"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<Box2dColliderSerialization>
    {
        static bool decode(const Node &node, Box2dColliderSerialization &rhs) {
            rhs.scale = node["size"].as<glm::vec3>();
            if (node["translate"].IsDefined())
                rhs.translate = node["translate"].as<glm::vec3>();

            return true;
        }
    };

    //grid_prefab_spawner
    template <>
    struct convert<GridSerialization> {
        static bool decode(const Node &node, GridSerialization &rhs) {
            rhs.spawnOffset = node["spawn_offset"].as<glm::vec3>();
            rhs.spawnStep = node["spawn_step"].as<glm::vec3>();
            rhs.grid = std::vector<std::vector<int>>();
            if (node["grid"]) {
                const auto grid = node["grid"];

                for (const auto &position : grid) {
                    std::vector<int> vec;

                    for (const auto &item : position) {
                        const auto value = item.as<int>();
                        vec.push_back(value);
                    }

                    rhs.grid.emplace_back(vec);
                }
            }
            return true;
        }
    };

    //grid_prefab_spawner
    template <>
    struct convert<GridPrefabSpawnerSerialization>
    {
        static bool decode(const Node &node, GridPrefabSpawnerSerialization &rhs) {
            rhs.prefabId = node["prefab_id"].as<std::string>();
            rhs.gridTag = node["grid_tag"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<PathFinderSerialization>
    {
        static bool decode(const Node &node, PathFinderSerialization &rhs) {
            rhs.gridTag = node["grid_tag"].as<std::string>();
            return true;
        }
    };

    //prefab_spawner
    template <>
    struct convert<PrefabSpawnerSerialization>
    {
        static bool decode(const Node &node, PrefabSpawnerSerialization &rhs) {
            rhs.prefabId = node["prefab_id"].as<std::string>();
            rhs.spawnTime = node["spawn_time"].as<float>();
            rhs.maxSpawn = node["max_spawn"].as<int>();
            rhs.positions = std::vector<glm::vec3>();
            if (node["positions"]) {
                auto positions = node["positions"];

                for (const auto &position : positions) {
                    const auto x = position[0].as<float>();
                    const auto y = position[1].as<float>();
                    const auto z = position[2].as<float>();
                    rhs.positions.emplace_back(x, y, z);
                }
            }

            return true;
        }
    };

    template <>
    struct convert<ParticleEmitterSerialization>
    {
        static bool decode(const Node &node, ParticleEmitterSerialization &rhs) {
            rhs.count = node["count"].as<size_t>();
            rhs.startVelocity = node["start_velocity"].as<glm::vec3>();
            rhs.endVelocity = node["end_velocity"].as<glm::vec3>();
            rhs.duration = node["duration"].as<float>();
            rhs.materialGuid = node["material_guid"].as<std::string>();
            rhs.startScale = node["start_scale"].as<float>();
            rhs.endScale = node["end_scale"].as<float>();
            rhs.startColor = node["start_color"].as<glm::vec4>();
            rhs.endColor = node["end_color"].as<glm::vec4>();
            return true;
        }
    };

    template <>
    struct convert<SpineColliderSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] SpineColliderSerialization &rhs) {
            return true;
        }
    };

    template <>
    struct convert<Rigidbody2dSerialization>
    {
        static bool decode(const Node &node, Rigidbody2dSerialization &rhs) {
            rhs.isDynamic = node["isDynamic"].as<bool>();
            return true;
        }
    };

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

        static bool decode(const Node &node, EntitySerialization &rhs) {
            if (node["guid"])
                rhs.Guid = node["guid"].as<std::string>();
            if (node["tag"])
                rhs.Tag = node["tag"].as<std::string>();
            if (node["layer"])
                rhs.Layer = node["layer"].as<int>();

            for (const auto &compNode : node["components"])
            {
                rhs.Components.push_back(createComponentFromYAML(compNode));
            }
            return true;
        }
    };

    template <>
    struct convert<PrefabAsset>
    {
        static bool decode(const Node &node, PrefabAsset &rhs)
        {
            rhs.Obj = node.as<EntitySerialization>();
            return true;
        }
    };

    template <>
    struct convert<SceneAsset>
    {
        static bool decode(const Node &node, SceneAsset &rhs)
        {
            rhs.Type = node["type"].as<std::string>();
            rhs.Entities = node["entities"].as<std::vector<EntitySerialization>>();
            return true;
        }
    };

    template <>
    struct convert<MetaAsset>
    {
        static bool decode(const Node &node, MetaAsset &rhs)
        {
            rhs.Guid = node["guid"].as<std::string>();
            rhs.Extension = node["extension"].as<std::string>();
            rhs.Type = node["type"].as<std::string>();

            return true;
        }
    };

    template <>
    struct convert<MaterialAsset>
    {
        static bool decode(const Node &node, MaterialAsset &rhs)
        {
            rhs.BlendMode = node["blend_mode"].as<int>();
            rhs.IsCulling = node["is_culling"].as<bool>();

#ifdef __EMSCRIPTEN__
            if (const auto shaderNode = node["shader_gles"]) {
                rhs.VertexShaderGuid = shaderNode["vertex"].as<std::string>();
                rhs.FragmentShaderGuid = shaderNode["fragment"].as<std::string>();
            }
#else
            if (const auto shaderNode = node["shader_opengl"]) {
                rhs.VertexShaderGuid = shaderNode["vertex"].as<std::string>();
                rhs.FragmentShaderGuid = shaderNode["fragment"].as<std::string>();
            }
#endif


            if (node["font"] && node["font"].IsScalar()) {
                rhs.Font = node["font"].as<std::string>();
            }

            if (node["image"] && node["image"].IsScalar()) {
                rhs.Image = node["image"].as<std::string>();
            }

            return true;
        }
    };

    template <>
    struct convert<ProjectAsset>
    {
        static bool decode(const Node &node, ProjectAsset &rhs)
        {
            rhs.Name = node["name"].as<std::string>();
            rhs.Scenes = node["scenes"].as<std::vector<std::string>>();
            rhs.Resolution = node["resolution"].as<std::vector<int>>();
            rhs.TargetFps = node["target_fps"].as<float>();
            return true;
        }
    };

    template <>
    struct convert<SpineAsset>
    {
        static bool decode(const Node &node, SpineAsset &rhs)
        {
            rhs.skeleton = node["skeleton"].as<std::string>();
            rhs.atlas = node["atlas"].as<std::string>();
            rhs.image = node["image"].as<std::string>();
            rhs.json = node["json"].as<std::string>();
            rhs.spineScale = node["spine_scale"].as<float>();
            rhs.moveAnimationName = node["move_animation_name"].as<std::string>();
            rhs.jumpAnimationName = node["jump_animation_name"].as<std::string>();
            rhs.hitAnimationName = node["hit_animation_name"].as<std::string>();
            rhs.idleAnimationName = node["idle_animation_name"].as<std::string>();
            return true;
        }
    };


    template <>
    struct convert<RectTransformFollowerSerialization>
    {
        static bool decode(const Node &node, RectTransformFollowerSerialization &rhs)
        {
            rhs.UseCreator = node["use_creator"].as<bool>();
            rhs.Offset = node["offset"].as<glm::vec2>();
            if (node["target"])
                rhs.Target = node["target"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<DestroyWithCreatorComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]]  DestroyWithCreatorComponentSerialization &rhs)
        {
            return true;
        }
    };

    template <>
    struct convert<IdleCharacterSerialization>
    {
        static bool decode(const Node &node, IdleCharacterSerialization &rhs)
        {
            rhs.IdleAnimation = node["idle_animation"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<UiButtonComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]]  UiButtonComponentSerialization &rhs)
        {
            return true;
        }
    };

    template <>
    struct convert<OnClickSceneLoaderSerialization>
    {
        static bool decode(const Node &node, OnClickSceneLoaderSerialization &rhs)
        {
            rhs.SceneGuid = node["scene_guid"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<TeamSerialization>
    {
        static bool decode(const Node &node, TeamSerialization &rhs)
        {
            rhs.Team = node["team"].as<int>();
            return true;
        }
    };

    template <>
    struct convert<GameStateData>
    {
        static bool decode(const Node &node, GameStateData &rhs)
        {
            rhs.WinScene = node["win_scene"].as<std::string>();
            rhs.LooseScene = node["lose_scene"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<UiButtonEffectSerialization>
    {
        static bool decode([[maybe_unused]]const Node &node, [[maybe_unused]] UiButtonEffectSerialization &rhs)
        {
            return true;
        }
    };

} // namespace YAML
