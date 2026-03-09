#include "core/src/plugin/plugin_registrar.hpp"

// Action
#include "src/action/example_noop_action.hpp"
#include "src/action/example_noop_action_serialization.hpp"
#include "src/action/example_noop_action_serialization_yaml.hpp"

// Condition
#include "src/condition/example_condition.hpp"
#include "src/condition/example_condition_serialization.hpp"
#include "src/condition/example_condition_serialization_yaml.hpp"

// Component
#include "src/component/example_component.hpp"
#include "src/component/example_component_serialization.hpp"
#include "src/component/example_component_serialization_yaml.hpp"

// System
#include "src/system/example_system.hpp"
#include "esc/esc_system_context.hpp"

// Asset Loader
#include "src/asset_loader/example_asset_loader.hpp"

struct ExamplePlugin {
    static void Register(PluginRegistries& registries) {

        // -- Asset Loader --
        // Initialize the ExampleAsset loader so AssetManager can load .txt files as ExampleAsset
        // Usage: auto asset = assetManager->LoadSourceByGuid<ExampleAsset>(guid);
        AssetLoader<ExampleAsset>::Init();

        // -- FSM Action --
        // Usage in .fsm: type: example_noop, label: "hello"
        registries.Actions.Register<ExampleNoopActionSerialization>("example_noop",
            [](const ExampleNoopActionSerialization& s, [[maybe_unused]] const EngineContext& ctx) {
                return std::make_unique<ExampleNoopAction>(s.Label);
            }
        );

        // -- FSM Condition --
        // Usage in .fsm: type: example_always_pass
        registries.Conditions.Register<ExampleAlwaysPassConditionSerialization>("example_always_pass",
            []([[maybe_unused]] const ExampleAlwaysPassConditionSerialization& s, [[maybe_unused]] const EngineContext& ctx) {
                return std::make_unique<ExampleAlwaysPassCondition>();
            }
        );

        // -- ECS Component --
        // Usage in .scene/.prefab: type: example_tag, tag: "my_tag"
        registries.Components.Register<ExampleTagComponentSerialization, ExampleTagComponent>("example_tag",
            [](const ExampleTagComponentSerialization& s) {
                return ExampleTagComponent{s.Tag};
            }
        );

        // -- ECS System --
        // Runs each frame, iterates entities with ExampleTagComponent
        // Priority 900 = runs after most systems
        registries.Systems.Register<ExampleSystem>("ExampleSystem", [](const EscSystemContext& ctx) {
            return std::make_unique<ExampleSystem>(*ctx.Registry);
        }, 900);
    }
};

REGISTER_PLUGIN(ExamplePlugin)
