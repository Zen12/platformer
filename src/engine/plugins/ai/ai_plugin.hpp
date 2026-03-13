#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "core/src/plugin/plugin_registry.hpp"
#include "src/register_ai_components.hpp"
#include "src/register_ai_systems.hpp"
#include "src/bt_asset.hpp"
#include "src/bt_asset_yaml.hpp"
#include "resource_factory.hpp"

struct AIPlugin {
    static void Register(PluginRegistries& registries) {
        // -- Resource Loaders --
        registries.Resources.RegisterLoader<BehaviorTreeDef>([](ResourceFactory& factory, const Guid& guid) -> std::shared_ptr<BehaviorTreeDef> {
            if (guid.IsEmpty())
                return {};
            if (const auto assetManager = factory.GetAssetManager().lock()) {
                return factory.GetCache()->GetOrLoad<BehaviorTreeDef>(guid, [&]() {
                    auto btAsset = assetManager->LoadAssetByGuid<BehaviorTreeAsset>(guid);
                    return std::make_shared<BehaviorTreeDef>(std::move(btAsset.Tree));
                });
            }
            return {};
        });

        // -- ECS Components --
        RegisterAIComponents(registries.Components);

        // -- ECS Systems --
        RegisterAISystems(registries.Systems);
    }
};
