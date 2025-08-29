#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <memory>

#include "../serialization/serialization_component.hpp"
#include "../serialization/serialization_asset.hpp"

// Helper to extract a map from a YAML node that is a sequence of single-key maps
inline YAML::Node sequenceToMap(const YAML::Node &seq)
{
    YAML::Node result(YAML::NodeType::Map);
    for (const auto &item : seq)
    {
        if (item.IsMap() && item.size() == 1)
        {
            for (const auto &kv : item)
            {
                result[kv.first] = kv.second;
            }
        }
    }
    return result;
}

namespace YAML
{

    template <>
    struct convert<glm::vec3>
    {
        static bool decode(const Node &node, glm::vec3 &rhs)
        {
            const auto map = sequenceToMap(node);
            rhs.x = map["x"].as<float>();
            rhs.y = map["y"].as<float>();
            rhs.z = map["z"].as<float>();
            return true;
        }
    };

    template <>
    struct convert<UiTextComponentSerialization>
    {
        static bool decode(const Node &node, UiTextComponentSerialization &rhs)
        {
            const auto map = sequenceToMap(node);
            rhs.MaterialGUID = map["material"].as<std::string>();
            rhs.Text = map["text"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<CharacterControllerComponentSerialization>
    {
        static bool decode(const Node &node, CharacterControllerComponentSerialization &rhs)
        {
            const auto map = sequenceToMap(node);
            rhs.MaxMovementSpeed = map["max_movement_speed"].as<float>();
            rhs.AccelerationSpeed = map["acceleration_speed"].as<float>();
            rhs.DecelerationSpeed = map["deceleration_speed"].as<float>();
            rhs.JumpHeigh = map["jump_heigh"].as<float>();
            rhs.JumpDuration = map["jump_duration"].as<float>();
            rhs.JumpDownMultiplier = map["jump_down_multiplier"].as<float>();
            rhs.AirControl = map["air_control"].as<float>();
            rhs.Damage = map["damage"].as<float>();
            return true;
        }
    };

    template <>
    struct convert<AiControllerComponentSerialization>
    {
        static bool decode(const Node &node, AiControllerComponentSerialization &rhs)
        {
            const auto map = sequenceToMap(node);
            rhs.MaxMovementSpeed = map["max_movement_speed"].as<float>();
            rhs.AccelerationSpeed = map["acceleration_speed"].as<float>();
            rhs.DecelerationSpeed = map["deceleration_speed"].as<float>();
            rhs.JumpHeigh = map["jump_heigh"].as<float>();
            rhs.JumpDuration = map["jump_duration"].as<float>();
            rhs.JumpDownMultiplier = map["jump_down_multiplier"].as<float>();
            rhs.AirControl = map["air_control"].as<float>();
            rhs.Damage = map["damage"].as<float>();
            rhs.AiTargetTransformTag = map["ai_target_transform_tag"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<HealthComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] HealthComponentSerialization &rhs)
        {
            const auto map = sequenceToMap(node);
            rhs.Health = map["health"].as<float>();

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
            const auto map = sequenceToMap(node);
            rhs.aspectPower = map["aspectPower"].as<float>();
            rhs.isPerspective = map["isPerspective"].as<bool>();
            return true;
        }
    };

    template <>
    struct convert<TransformComponentSerialization>
    {
        static bool decode(const Node &node, TransformComponentSerialization &rhs)
        {
            const auto map = sequenceToMap(node);
            rhs.position = map["position"].as<glm::vec3>();
            rhs.rotation = map["rotation"].as<glm::vec3>();
            rhs.scale = map["scale"].as<glm::vec3>();
            return true;
        }
    };

    template <>
    struct convert<SpineRenderComponentSerialization>
    {
        static bool decode(const Node &node, SpineRenderComponentSerialization &rhs)
        {
            const auto map = sequenceToMap(node);
            rhs.SpineGuid = map["spine_data"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<MeshRendererComponentSerialization>
    {
        static bool decode(const Node &node, MeshRendererComponentSerialization &rhs)
        {
            const auto map = sequenceToMap(node);
            rhs.MaterialGuid = map["material"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<Light2dComponentSerialization>
    {
        static bool decode(const Node &node, Light2dComponentSerialization &rhs) {
            const auto map = sequenceToMap(node);
            rhs.CenterTransform = map["center"].as<std::string>();
            rhs.Offset = map["offset"].as<glm::vec3>();
            return true;
        }
    };

    template <>
    struct convert<SpriteRenderComponentSerialization>
    {
        static bool decode(const Node &node, SpriteRenderComponentSerialization &rhs) {
            const auto map = sequenceToMap(node);
            rhs.MaterialGuid = map["material"].as<std::string>();
            rhs.SpriteGuid = map["image"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<UiImageComponentSerialization>
    {
        static bool decode(const Node &node, UiImageComponentSerialization &rhs) {
            const auto map = sequenceToMap(node);
            rhs.MaterialGuid = map["material"].as<std::string>();
            rhs.SpriteGuid = map["image"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<RectTransformComponentSerialization>
    {
        static bool decode(const Node &node, RectTransformComponentSerialization &rhs) {

            const auto map = sequenceToMap(node);

            if (!map.IsMap())
            {
                std::cerr << "Expected a map node.\n";
                return false;
            }


            for (const auto &kv : map)
            {
                const auto key = kv.first.as<std::string>();
                YAML::Node list = kv.second;

                auto x = 0.0f;
                auto y = 0.0f;
                auto value =  0.0f;

                for (const auto &item : list) {
                    x = item["x"] ? item["x"].as<float>() : x;
                    y = item["y"] ? item["y"].as<float>() : y;
                    value = item["value"] ? item["value"].as<float>() : value;
                }

                rhs.Layouts.emplace_back(key, value, x, y);
            }

            return true;
        }
    };

    template <>
    struct convert<SpriteComponentSerialization>
    {
        static bool decode(const Node &node, SpriteComponentSerialization &rhs) {
            const auto map = sequenceToMap(node);
            rhs.Path = map["path"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<Box2dColliderSerialization>
    {
        static bool decode(const Node &node, Box2dColliderSerialization &rhs) {
            const auto map = sequenceToMap(node);
            rhs.scale = map["size"].as<glm::vec3>();
            if (map["translate"].IsDefined())
                rhs.translate = map["translate"].as<glm::vec3>();

            return true;
        }
    };

    //grid_prefab_spawner
    template <>
    struct convert<GridSerialization> {
        static bool decode(const Node &node, GridSerialization &rhs) {
            const auto map = sequenceToMap(node);
            rhs.spawnOffset = map["spawn_offset"].as<glm::vec3>();
            rhs.spawnStep = map["spawn_step"].as<glm::vec3>();
            rhs.grid = std::vector<std::vector<int>>();
            if (map["grid"]) {
                const auto grid = map["grid"];

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
            const auto map = sequenceToMap(node);
            rhs.prefabId = map["prefab_id"].as<std::string>();
            rhs.gridTag = map["grid_tag"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<PathFinderSerialization>
    {
        static bool decode(const Node &node, PathFinderSerialization &rhs) {
            const auto map = sequenceToMap(node);
            rhs.gridTag = map["grid_tag"].as<std::string>();
            return true;
        }
    };

    //prefab_spawner
    template <>
    struct convert<PrefabSpawnerSerialization>
    {
        static bool decode(const Node &node, PrefabSpawnerSerialization &rhs) {
            const auto map = sequenceToMap(node);
            rhs.prefabId = map["prefab_id"].as<std::string>();
            rhs.spawnTime = map["spawn_time"].as<float>();
            rhs.maxSpawn = map["max_spawn"].as<int>();
            rhs.positions = std::vector<glm::vec3>();
            if (map["positions"]) {
                auto positions = map["positions"];

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
    struct convert<Rigidbody2dSerialization>
    {
        static bool decode(const Node &node, Rigidbody2dSerialization &rhs) {
            const auto map = sequenceToMap(node);
            rhs.isDynamic = map["isDynamic"].as<bool>();
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

        static  std::unique_ptr<ComponentSerialization> createComponentFromYAML(const YAML::Node &node)
        {
            const auto nodeMap = sequenceToMap(node);
            const auto type = nodeMap["type"].as<std::string>();
            const YAML::Node &data = nodeMap["data"];
            const auto map = sequenceToMap(data);

            if (type == "camera") {
                return Parse<CameraComponentSerialization>(data);
            } else if (type == "transform") {
                return Parse<TransformComponentSerialization>(data);
            } else if (type == "rect_transform") {
                return Parse<RectTransformComponentSerialization>(data);
            } else if (type == "ui_image") {
                return Parse<UiImageComponentSerialization>(data);
            } if (type == "ui_text") {
                return Parse<UiTextComponentSerialization>(data);
            } else if (type == "sprite_renderer") {
                return Parse<SpriteRenderComponentSerialization>(data);
            } else if (type == "box_collider") {
                return Parse<Box2dColliderSerialization>(data);
            } else if (type == "rigidbody2d") {
                return Parse<Rigidbody2dSerialization>(data);
            } else if (type == "light_2d") {
                return Parse<Light2dComponentSerialization>(data);
            } if (type == "mesh_renderer") {
                return Parse<MeshRendererComponentSerialization>(data);
            } if (type == "spine_renderer") {
                return Parse<SpineRenderComponentSerialization>(data);
            } if (type == "character_controller") {
                return Parse<CharacterControllerComponentSerialization>(data);
            } else if (type == "ai_controller") {
                return Parse<AiControllerComponentSerialization>(data);
            } else if (type == "ui_show_fps") {
                return Parse<ShowFpsComponentSerialization>(data);
            } else if (type == "health_component") {
                return Parse<HealthComponentSerialization>(data);
            } else if (type == "prefab_spawner") {
                return Parse<PrefabSpawnerSerialization>(data);
            }else if (type == "grid_prefab_spawner") {
                return Parse<GridPrefabSpawnerSerialization>(data);
            } else if (type == "grid") {
                return Parse<GridSerialization>(data);
            } else if (type == "astar_path_finder") {
                return Parse<PathFinderSerialization>(data);
            }

            throw std::runtime_error("Unknown component type: " + type);
        }

        static bool decode(const Node &node, EntitySerialization &rhs)
        {
            if (const auto objNode = node["obj"])
            {
                const auto nodeMap = sequenceToMap(objNode);

                if (nodeMap["guid"])
                    rhs.Guid = nodeMap["guid"].as<std::string>();
                if (nodeMap["tag"]) {
                    rhs.Tag = nodeMap["tag"].as<std::string>();
                }

                for (const auto &compNode : nodeMap["components"])
                {
                    if (const auto componentNode = compNode["component"])
                    {
                        rhs.Components.push_back(createComponentFromYAML(componentNode));
                    }
                }
            }
            return true;
        }
    };

    template <>
    struct convert<PrefabAsset>
    {
        static bool decode(const Node &node, PrefabAsset &rhs)
        {
            const auto nodeMap = sequenceToMap(node);
            rhs.Name = nodeMap["name"].as<std::string>();
            rhs.Obj = nodeMap.as<EntitySerialization>();
            return true;
        }
    };

    template <>
    struct convert<SceneAsset>
    {
        static bool decode(const Node &node, SceneAsset &rhs)
        {
            rhs.Name = node["name"].as<std::string>();
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
            rhs.Name = node["name"].as<std::string>();
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
            rhs.Name = node["name"].as<std::string>();
            if (const auto shaderNode = node["shader"]) {
                rhs.VertexShaderGuid = shaderNode["vertex"].as<std::string>();
                rhs.FragmentShaderGuid = shaderNode["fragment"].as<std::string>();
            }

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
            rhs.Name = node["name"].as<std::string>();
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

} // namespace YAML
