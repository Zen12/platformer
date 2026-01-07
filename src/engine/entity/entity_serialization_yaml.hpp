#pragma once

#include "entity_serialization.hpp"
#include "../math/glm_vec_yaml.hpp"
#include "../esc/transform/transform_component_serialization.hpp"
#include "../esc/transform/transform_component_serialization_yaml.hpp"
#include "../esc/mesh_renderer/mesh_renderer_component_serialization.hpp"
#include "../esc/mesh_renderer/mesh_renderer_component_serialization_yaml.hpp"
#include "../esc/skinned_mesh_renderer/skinned_mesh_renderer_component_serialization.hpp"
#include "../esc/skinned_mesh_renderer/skinned_mesh_renderer_component_serialization_yaml.hpp"
#include "../esc/camera/camera_component_serialization.hpp"
#include "../esc/camera/camera_component_serialization_yaml.hpp"
#include "../esc/camera/camera_controller_component_serialization.hpp"
#include "../esc/camera/camera_controller_component_serialization_yaml.hpp"
#include "../esc/camera/top_down_camera_component_serialization.hpp"
#include "../esc/camera/top_down_camera_component_serialization_yaml.hpp"
#include "../esc/player_controller/player_controller_component_serialization.hpp"
#include "../esc/player_controller/player_controller_component_serialization_yaml.hpp"
#include "../esc/animation/simple_animation_component_serialization.hpp"
#include "../esc/animation/simple_animation_component_serialization_yaml.hpp"
#include "../esc/animation/fsm_animation_component_serialization.hpp"
#include "../esc/animation/fsm_animation_component_serialization_yaml.hpp"
#include "../esc/navmesh_agent/navmesh_agent_component_serialization.hpp"
#include "../esc/navmesh_agent/navmesh_agent_component_serialization_yaml.hpp"
#include "../esc/spawner/spawner_component_serialization.hpp"
#include "../esc/spawner/spawner_component_serialization_yaml.hpp"
#include "../ai/bt_component_serialization.hpp"
#include "../ai/bt_component_serialization_yaml.hpp"
#include "../esc/directional_light/directional_light_component_serialization.hpp"
#include "../esc/directional_light/directional_light_component_serialization_yaml.hpp"

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
                    rhs.Components.push_back(DecodeComponents(comp));
                }
            }

            return true;
        }

        static std::unique_ptr<ComponentSerialization> DecodeComponents(const YAML::Node& node) {

            const auto type = node["type"].as<std::string>();

            using ComponentSerialization = std::function<std::unique_ptr<ComponentSerialization>(const YAML::Node&)>;
            static const std::unordered_map<std::string, ComponentSerialization> pairs = {
                { "camera",                [](const YAML::Node& n){ return Parse<CameraComponentSerialization>(n); } },
                { "camera_controller",     [](const YAML::Node& n){ return Parse<CameraControllerComponentSerialization>(n); } },
                { "top_down_camera",       [](const YAML::Node& n){ return Parse<TopDownCameraComponentSerialization>(n); } },
                { "player_controller",     [](const YAML::Node& n){ return Parse<PlayerControllerComponentSerialization>(n); } },
                { "transform",             [](const YAML::Node& n){ return Parse<TransformComponentSerialization>(n); } },
                { "mesh_renderer",         [](const YAML::Node& n){ return Parse<MeshRendererComponentSerialization>(n); } },
                { "skinned_mesh_renderer", [](const YAML::Node& n){ return Parse<SkinnedMeshRendererComponentSerialization>(n); } },
                { "simple_animation",      [](const YAML::Node& n){ return Parse<SimpleAnimationComponentSerialization>(n); } },
                { "fsm_animation",         [](const YAML::Node& n){ return Parse<FsmAnimationComponentSerialization>(n); } },
                { "navmesh_agent",         [](const YAML::Node& n){ return Parse<NavmeshAgentComponentSerialization>(n); } },
                { "spawner",               [](const YAML::Node& n){ return Parse<SpawnerComponentSerialization>(n); } },
                { "behavior_tree",         [](const YAML::Node& n){ return Parse<BehaviorTreeComponentSerialization>(n); } },
                { "directional_light",     [](const YAML::Node& n){ return Parse<DirectionalLightComponentSerialization>(n); } },
            };

            if (const auto it = pairs.find(type); it != pairs.end()) {
                return it->second(node);
            }

            throw std::runtime_error("Unknown component type: " + type);
        }

    };
}
