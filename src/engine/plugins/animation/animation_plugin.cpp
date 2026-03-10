#define GLM_ENABLE_EXPERIMENTAL
#include "core/src/plugin/plugin_registrar.hpp"
#include "esc/esc_system_context.hpp"
#include "src/animation/animation_asset_loader.hpp"

// Actions
#include "src/action/register_animation_actions.hpp"

// Components
#include "src/component/simple_animation_component.hpp"
#include "src/component/simple_animation_component_serialization.hpp"
#include "src/component/simple_animation_component_serialization_yaml.hpp"
#include "src/component/fsm_animation_component.hpp"
#include "src/component/fsm_animation_component_serialization.hpp"
#include "src/component/fsm_animation_component_serialization_yaml.hpp"

// Systems
#include "src/system/simple_animation_system.hpp"
#include "src/system/fsm_animation_system.hpp"

#include "scene.hpp"
#include "resource_cache.hpp"

struct AnimationPlugin {
    static void Register(PluginRegistries& registries) {
        AssetLoader<AnimationData>::Init();

        // -- FSM Actions --
        RegisterAnimationActions(registries.Actions);

        // -- ECS Components --

        // simple_animation
        registries.Components.Register<SimpleAnimationComponentSerialization, SimpleAnimationComponent>("simple_animation",
            [](const SimpleAnimationComponentSerialization& s) {
                return SimpleAnimationComponent(s.AnimationGuid);
            }
        );

        // fsm_animation
        registries.Components.Register<FsmAnimationComponentSerialization>("fsm_animation",
            [](entt::registry& reg, entt::entity entity, const FsmAnimationComponentSerialization& s) {
                FsmAnimationComponent fsmAnimComp(s.FsmGuid);
                fsmAnimComp.Loop = s.Loop;
                fsmAnimComp.Blender.Configure(s.BlenderConfig);
                reg.emplace<FsmAnimationComponent>(entity, fsmAnimComp);
            }
        );

        // -- ECS Systems --

        // SimpleAnimationSystem: priority 190
        registries.Systems.Register<SimpleAnimationSystem>("SimpleAnimationSystem", [](const EscSystemContext& ctx) {
            const auto reg = ctx.Registry;
            return std::make_unique<SimpleAnimationSystem>(
                reg->view<SimpleAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
                reg->view<DeltaTimeComponent>(), ctx.ResourceCache);
        }, 190);

        // FsmAnimationSystem: priority 200
        registries.Systems.Register<FsmAnimationSystem>("FsmAnimationSystem", [](const EscSystemContext& ctx) {
            const auto scenePtr = ctx.Scene.lock();
            if (!scenePtr) return std::unique_ptr<FsmAnimationSystem>(nullptr);
            const auto reg = ctx.Registry;
            return std::make_unique<FsmAnimationSystem>(
                reg->view<FsmAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
                reg->view<DeltaTimeComponent>(), scenePtr, ctx.ResourceCache, *reg);
        }, 200);
    }
};

REGISTER_PLUGIN(AnimationPlugin)
