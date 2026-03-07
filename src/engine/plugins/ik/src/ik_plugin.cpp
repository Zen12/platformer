#define GLM_ENABLE_EXPERIMENTAL
#include "core/src/plugin/plugin_registrar.hpp"
#include "esc/esc_system_context.hpp"

// Components
#include "component/ik_aim_component.hpp"
#include "component/ik_aim_component_serialization.hpp"
#include "component/ik_aim_component_serialization_yaml.hpp"

// Systems
#include "system/ik_aim_system.hpp"

#include "scene.hpp"

struct IKPlugin {
    static void Register(PluginRegistries& registries) {

        // -- ECS Components --

        // ik_aim
        registries.Components.Register<IKAimComponentSerialization>("ik_aim",
            [](entt::registry& reg, entt::entity entity, const IKAimComponentSerialization& s) {
                IKAimComponent ikComp;
                ikComp.Enabled = s.Enabled;
                ikComp.BlendWeight = s.BlendWeight;
                ikComp.BlendSpeed = s.BlendSpeed;
                ikComp.TorsoMaxYaw = s.TorsoMaxYaw;
                ikComp.TorsoMaxPitch = s.TorsoMaxPitch;
                ikComp.SpineWeight = s.SpineWeight;
                ikComp.Spine1Weight = s.Spine1Weight;
                ikComp.ChestWeight = s.ChestWeight;
                ikComp.RightHandOffset = s.RightHandOffset;
                ikComp.LeftHandOffset = s.LeftHandOffset;
                ikComp.LeftHandGripPoint = s.LeftHandGripPoint;
                ikComp.RightElbowHint = s.RightElbowHint;
                ikComp.LeftElbowHint = s.LeftElbowHint;
                ikComp.RotateCharacterToAim = s.RotateCharacterToAim;
                ikComp.CharacterRotationSpeed = s.CharacterRotationSpeed;
                reg.emplace<IKAimComponent>(entity, ikComp);
            }
        );

        // -- ECS Systems --

        // IKAimSystem: priority 210
        registries.Systems.Register<IKAimSystem>("IKAimSystem", [](const EscSystemContext& ctx) {
            const auto scenePtr = ctx.Scene.lock();
            if (!scenePtr) return std::unique_ptr<IKAimSystem>(nullptr);
            const auto reg = ctx.Registry;
            return std::make_unique<IKAimSystem>(
                reg->view<IKAimComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
                reg->view<DeltaTimeComponent>(),
                reg->view<CameraComponentV2, TransformComponentV2>(),
                reg->view<WindowComponent>(),
                scenePtr,
                *reg);
        }, 210);
    }
};

REGISTER_PLUGIN(IKPlugin)
