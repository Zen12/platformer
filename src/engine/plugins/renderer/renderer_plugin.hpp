#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "core/src/plugin/plugin_registry.hpp"
#include "src/action/register_renderer_actions.hpp"
#include "src/register_renderer_systems.hpp"
#include "src/register_renderer_components.hpp"
#include "src/register_renderer_resources.hpp"
#include "src/texture/texture_asset_loader.hpp"
#include "src/mesh/mesh_asset_loader.hpp"

struct RendererPlugin {
    static void Register(PluginRegistries& registries) {
        AssetLoader<Texture>::Init();
        AssetLoader<MeshData>::Init();

        RegisterRendererActions(registries.Actions);
        RegisterRendererComponents(registries.Components);
        RegisterRendererSystems(registries.Systems);
        RegisterRendererResources(registries.Resources);
    }
};
