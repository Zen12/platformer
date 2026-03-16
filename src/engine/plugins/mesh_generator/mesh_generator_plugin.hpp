#pragma once
#include "core/src/plugin/plugin_registry.hpp"
#include "src/meshgen_resource_generator.hpp"
#include "src/component/gen_mesh_renderer_component.hpp"
#include "src/component/gen_mesh_renderer_component_serialization.hpp"
#include "src/component/gen_mesh_renderer_component_serialization_yaml.hpp"
#include "src/system/gen_mesh_render_system.hpp"
#include "esc/esc_system_context.hpp"

struct MeshGeneratorPlugin {
    static void Register(PluginRegistries& registries) {
        RegisterMeshGenResourceGenerator(registries.Resources);

        registries.Components.Register<GenMeshRendererComponentSerialization, GenMeshRendererComponent>("gen_mesh_renderer",
            [](const GenMeshRendererComponentSerialization& s) {
                return GenMeshRendererComponent(s);
            }
        );

        registries.Systems.Register<GenMeshRenderSystem>("GenMeshRenderSystem", [](const EscSystemContext& ctx) {
            const auto reg = ctx.Registry;
            return std::make_unique<GenMeshRenderSystem>(
                reg->view<GenMeshRendererComponent, TransformComponentV2>(),
                reg->view<CameraComponentV2>(),
                reg->view<RenderBufferComponent>(),
                ctx.ResourceFactory);
        }, 217);
    }
};
