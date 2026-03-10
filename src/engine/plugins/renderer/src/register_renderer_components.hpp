#pragma once

#include "entity/component_registry.hpp"

#include "transform/transform_component.hpp"
#include "transform/transform_component_serialization.hpp"
#include "transform/transform_component_serialization_yaml.hpp"

#include "camera/camera_component.hpp"
#include "camera/camera_component_serialization.hpp"
#include "camera/camera_component_serialization_yaml.hpp"

#include "camera/camera_controller_component.hpp"
#include "camera/camera_controller_component_serialization.hpp"
#include "camera/camera_controller_component_serialization_yaml.hpp"

#include "camera/top_down_camera_component.hpp"
#include "camera/top_down_camera_component_serialization.hpp"
#include "camera/top_down_camera_component_serialization_yaml.hpp"

#include "mesh_renderer/mesh_renderer_component.hpp"
#include "mesh_renderer/mesh_renderer_component_serialization.hpp"
#include "mesh_renderer/mesh_renderer_component_serialization_yaml.hpp"

#include "skinned_mesh_renderer/skinned_mesh_renderer_component.hpp"
#include "skinned_mesh_renderer/skinned_mesh_renderer_component_serialization.hpp"
#include "skinned_mesh_renderer/skinned_mesh_renderer_component_serialization_yaml.hpp"

#include "directional_light/directional_light_component.hpp"
#include "directional_light/directional_light_component_serialization.hpp"
#include "directional_light/directional_light_component_serialization_yaml.hpp"

#include "particle_emitter/particle_emitter_component.hpp"
#include "particle_emitter/particle_emitter_component_serialization.hpp"
#include "particle_emitter/particle_emitter_component_serialization_yaml.hpp"

inline void RegisterRendererComponents(ComponentRegistry& registry) {
    // transform
    registry.Register<TransformComponentSerialization, TransformComponentV2>("transform",
        [](const TransformComponentSerialization& s) {
            return TransformComponentV2(s);
        }
    );

    // camera
    registry.Register<CameraComponentSerialization, CameraComponentV2>("camera",
        [](const CameraComponentSerialization& s) {
            return CameraComponentV2(s);
        }
    );

    // camera_controller
    registry.Register<CameraControllerComponentSerialization>("camera_controller",
        [](entt::registry& reg, entt::entity entity, const CameraControllerComponentSerialization& s) {
            CameraControllerComponent controller;
            controller.SetMoveSpeed(s.MoveSpeed);
            controller.SetMouseSensitivity(s.MouseSensitivity);
            reg.emplace<CameraControllerComponent>(entity, controller);
        }
    );

    // top_down_camera
    registry.Register<TopDownCameraComponentSerialization, TopDownCameraComponent>("top_down_camera",
        [](const TopDownCameraComponentSerialization& s) {
            return TopDownCameraComponent(s.TargetTag,
                s.OffsetPosition, s.OffsetRotation,
                s.MaxLookAhead, s.SmoothSpeed,
                s.MaxMoveSpeed);
        }
    );

    // mesh_renderer
    registry.Register<MeshRendererComponentSerialization, MeshRendererComponent>("mesh_renderer",
        [](const MeshRendererComponentSerialization& s) {
            return MeshRendererComponent(s);
        }
    );

    // skinned_mesh_renderer
    registry.Register<SkinnedMeshRendererComponentSerialization, SkinnedMeshRendererComponent>("skinned_mesh_renderer",
        [](const SkinnedMeshRendererComponentSerialization& s) {
            return SkinnedMeshRendererComponent(s);
        }
    );

    // directional_light
    registry.Register<DirectionalLightComponentSerialization, DirectionalLightComponent>("directional_light",
        [](const DirectionalLightComponentSerialization& s) {
            return DirectionalLightComponent(s);
        }
    );

    // particle_emitter
    registry.Register<ParticleEmitterComponentSerialization, ParticleEmitterComponent>("particle_emitter",
        [](const ParticleEmitterComponentSerialization& s) {
            return ParticleEmitterComponent(s);
        }
    );
}
