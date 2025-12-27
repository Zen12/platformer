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
#include "../esc/animation/simple_animation_component_serialization.hpp"
#include "../esc/animation/simple_animation_component_serialization_yaml.hpp"
#include "../esc/ozz_animation/ozz_animation_component_serialization.hpp"
#include "../esc/ozz_animation/ozz_animation_component_serialization_yaml.hpp"
#include "../esc/ozz_animation/ozz_skeleton_component_serialization.hpp"
#include "../esc/ozz_animation/ozz_skeleton_component_serialization_yaml.hpp"
#include "../esc/ozz_animation/ozz_ik_component_serialization.hpp"
#include "../esc/ozz_animation/ozz_ik_component_serialization_yaml.hpp"
#include "../esc/ozz_animation/mouse_ik_controller_component_serialization.hpp"
#include "../esc/ozz_animation/mouse_ik_controller_component_serialization_yaml.hpp"

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
                { "transform",             [](const YAML::Node& n){ return Parse<TransformComponentSerialization>(n); } },
                { "mesh_renderer",         [](const YAML::Node& n){ return Parse<MeshRendererComponentSerialization>(n); } },
                { "skinned_mesh_renderer", [](const YAML::Node& n){ return Parse<SkinnedMeshRendererComponentSerialization>(n); } },
                { "simple_animation",      [](const YAML::Node& n){ return Parse<SimpleAnimationComponentSerialization>(n); } },
                { "ozz_animation",         [](const YAML::Node& n){ return Parse<OzzAnimationComponentSerialization>(n); } },
                { "ozz_skeleton",          [](const YAML::Node& n){ return Parse<OzzSkeletonComponentSerialization>(n); } },
                { "ozz_ik",                [](const YAML::Node& n){ return Parse<OzzIKComponentSerialization>(n); } },
                { "mouse_ik_controller",   [](const YAML::Node& n){ return Parse<MouseIKControllerComponentSerialization>(n); } },
            };

            if (const auto it = pairs.find(type); it != pairs.end()) {
                return it->second(node);
            }

            throw std::runtime_error("Unknown component type: " + type);
        }

    };
}
