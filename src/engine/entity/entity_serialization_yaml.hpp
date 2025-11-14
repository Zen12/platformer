#pragma once

#include "entity_serialization.hpp"
#include "../math/glm_vec_yaml.hpp"
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
#include "../renderer/mesh/mesh_renderer_component_serialization.hpp"
#include "../renderer/mesh/mesh_renderer_component_serialization_yaml.hpp"
#include "../renderer/particles/particle_emitter_component_serialization.hpp"
#include "../renderer/particles/particle_emitter_component_serialization_yaml.hpp"
#include "../renderer/camera/camera_component_serialization.hpp"
#include "../renderer/camera/camera_component_serialization_yaml.hpp"
#include "../nav_mesh/astar_finder_component_serialization.hpp"
#include "../nav_mesh/astar_finder_component_serialization_yaml.hpp"
#include "../nav_mesh/path_mover_component_serialization.hpp"
#include "../nav_mesh/path_mover_component_serialization_yaml.hpp"

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
                { "rect_transform",        [](const YAML::Node& n){ return Parse<RectTransformComponentSerialization>(n); } },
                { "ui_image",              [](const YAML::Node& n){ return Parse<UiImageComponentSerialization>(n); } },
                { "ui_button",             [](const YAML::Node& n){ return Parse<UiButtonComponentSerialization>(n); } },
                { "ui_text",               [](const YAML::Node& n){ return Parse<UiTextComponentSerialization>(n); } },
                { "sprite_renderer",       [](const YAML::Node& n){ return Parse<SpriteRenderComponentSerialization>(n); } },
                { "mesh_renderer",         [](const YAML::Node& n){ return Parse<MeshRendererComponentSerialization>(n); } },
                { "ui_show_fps",           [](const YAML::Node& n){ return Parse<ShowFpsComponentSerialization>(n); } },
                { "astar_path_finder",     [](const YAML::Node& n){ return Parse<PathFinderSerialization>(n); } },
                { "particle_emitter",      [](const YAML::Node& n){ return Parse<ParticleEmitterSerialization>(n); } },
                { "on_click_scene_loader", [](const YAML::Node& n){ return Parse<OnClickSceneLoaderSerialization>(n); } },
                { "path_mover",            [](const YAML::Node& n){ return Parse<PathMoverComponentSerialization>(n); } },
                { "ui_button_effect",      [](const YAML::Node& n){ return Parse<UiButtonEffectSerialization>(n); } },
            };

            if (const auto it = pairs.find(type); it != pairs.end()) {
                return it->second(node);
            }

            throw std::runtime_error("Unknown component type: " + type);
        }

    };
}
