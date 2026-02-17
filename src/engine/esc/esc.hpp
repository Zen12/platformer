#pragma once

#include "resource_cache.hpp"
#include "profiler.hpp"
#include "animation/simple_animation_component.hpp"
#include "animation/simple_animation_component_serialization.hpp"
#include "animation/simple_animation_system.hpp"
#include "animation/fsm_animation_component.hpp"
#include "animation/fsm_animation_component_serialization.hpp"
#include "animation/fsm_animation_system.hpp"
#include "camera/camera_component.hpp"
#include "camera/camera_system.hpp"
#include "camera/camera_controller_component.hpp"
#include "camera/camera_controller_component_serialization.hpp"
#include "camera/camera_controller_system.hpp"
#include "camera/top_down_camera_component.hpp"
#include "camera/top_down_camera_component_serialization.hpp"
#include "camera/top_down_camera_system.hpp"
#include "player_controller/player_controller_component.hpp"
#include "player_controller/player_controller_component_serialization.hpp"
#include "player_controller/player_controller_system.hpp"
#include "entt/entt.hpp"
#include "mesh_renderer/mesh_renderer_component.hpp"
#include "mesh_renderer/mesh_render_system.hpp"
#include "skinned_mesh_renderer/skinned_mesh_renderer_component.hpp"
#include "skinned_mesh_renderer/skinned_mesh_render_system.hpp"
#include "tag/tag_component.hpp"
#include "time/time_component.hpp"
#include "transform/transform_component.hpp"
#include "window/window_component.hpp"
#include "window/window_system.hpp"
#include "navmesh_agent/navmesh_agent_component.hpp"
#include "navmesh_agent/navmesh_agent_component_serialization.hpp"
#include "navmesh_agent/navmesh_agent_system.hpp"
#include "navmesh_path_renderer/navmesh_path_render_system.hpp"
#include "navmesh_debug_renderer/navmesh_debug_render_system.hpp"
#include "spawner/spawner_component.hpp"
#include "spawner/spawner_component_serialization.hpp"
#include "spawner/spawner_system.hpp"
#include "bt_component.hpp"
#include "bt_component_serialization.hpp"
#include "bt_system.hpp"
#include "directional_light/directional_light_component.hpp"
#include "directional_light/directional_light_component_serialization.hpp"
#include "directional_light/directional_light_system.hpp"
#include "health/health_component.hpp"
#include "health/health_component_serialization.hpp"
#include "health/health_system.hpp"
#include "audio_source/audio_source_component.hpp"
#include "audio_source/audio_system.hpp"
#include "audio_listener/audio_listener_component.hpp"
#include "particle_emitter/particle_emitter_component.hpp"
#include "particle_emitter/particle_emitter_component_serialization.hpp"
#include "particle_emitter/particle_system.hpp"
#include "particle_emitter/particle_render_system.hpp"
#include "ik_aim/ik_aim_component.hpp"
#include "ik_aim/ik_aim_component_serialization.hpp"
#include "ik_aim/ik_aim_system.hpp"

#include "entity/component_registry.hpp"


class EscSystem {
    const std::weak_ptr<Scene> _scene;
    std::vector<std::unique_ptr<ISystem>> _systems;
    const ComponentRegistry* _componentRegistry = nullptr;

public:
    explicit EscSystem(const std::weak_ptr<Scene> &scene, const ComponentRegistry* componentRegistry = nullptr)
    : _scene(scene), _componentRegistry(componentRegistry) {

        if (const auto &scenePtr = _scene.lock()) {

            const auto registry = scenePtr->GetEntityRegistry();
             const auto &systemEntity = registry->create();

            registry->view<WindowComponent>()->emplace(systemEntity, WindowComponent());
            registry->view<DeltaTimeComponent>()->emplace(systemEntity, DeltaTimeComponent());
        }
    }

    void LoadEntities(const std::vector<EntitySerialization> &serialization) {
        if (const auto &scenePtr = _scene.lock()) {

            const auto registry = scenePtr->GetEntityRegistry();
            const auto &tagView = registry->view<TagComponent>();


            for (const auto & entitySerialization : serialization) {
                const auto &entity = registry->create();
                tagView->emplace(entity, entitySerialization.Tag);

                for (const auto& component : entitySerialization.Components) {
                    // Check plugin component registry first
                    if (_componentRegistry && !component->ComponentType.empty() &&
                        _componentRegistry->Emplace(component->ComponentType, *registry, entity, component.get())) {
                        continue;
                    }

                    if (const auto &cameraSerialization = dynamic_cast<CameraComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<CameraComponentV2>();
                        view->emplace(entity, *cameraSerialization);
                    }else if (const auto &transformSerialization = dynamic_cast<TransformComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<TransformComponentV2>();
                        view->emplace(entity, *transformSerialization);
                    } if (const auto &meshSerialization = dynamic_cast<MeshRendererComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<MeshRendererComponent>();
                        view->emplace(entity, *meshSerialization);
                    } else if (const auto &skinnedMeshSerialization = dynamic_cast<SkinnedMeshRendererComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<SkinnedMeshRendererComponent>();
                        view->emplace(entity, *skinnedMeshSerialization);
                    } else if (const auto &animationSerialization = dynamic_cast<SimpleAnimationComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<SimpleAnimationComponent>();
                        SimpleAnimationComponent animComp(animationSerialization->AnimationGuid);
                        view->emplace(entity, animComp);
                    } else if (const auto &fsmAnimationSerialization = dynamic_cast<FsmAnimationComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<FsmAnimationComponent>();
                        FsmAnimationComponent fsmAnimComp(fsmAnimationSerialization->FsmGuid);
                        fsmAnimComp.Loop = fsmAnimationSerialization->Loop;
                        fsmAnimComp.Blender.Configure(fsmAnimationSerialization->BlenderConfig);
                        view->emplace(entity, fsmAnimComp);
                    } else if (const auto &cameraControllerSerialization = dynamic_cast<CameraControllerComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<CameraControllerComponent>();
                        CameraControllerComponent controller;
                        controller.SetMoveSpeed(cameraControllerSerialization->MoveSpeed);
                        controller.SetMouseSensitivity(cameraControllerSerialization->MouseSensitivity);
                        view->emplace(entity, controller);
                    } else if (const auto &navmeshAgentSerialization = dynamic_cast<NavmeshAgentComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<NavmeshAgentComponent>();
                        view->emplace(entity, *navmeshAgentSerialization);
                    } else if (const auto &spawnerSerialization = dynamic_cast<SpawnerComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<SpawnerComponent>();
                        SpawnerComponent spawner(spawnerSerialization->PrefabGuid, spawnerSerialization->SpawnCount, spawnerSerialization->SpawnPositions, spawnerSerialization->SpawnOnNavmesh, spawnerSerialization->SpawnOnAllCells, spawnerSerialization->ElevationHeight, spawnerSerialization->YOffset);
                        view->emplace(entity, spawner);
                    } else if (const auto &btSerialization = dynamic_cast<BehaviorTreeComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<BehaviorTreeComponent>();
                        view->emplace(entity, *btSerialization);
                    } else if (const auto &topDownSerialization = dynamic_cast<TopDownCameraComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<TopDownCameraComponent>();
                        TopDownCameraComponent topDown(topDownSerialization->TargetTag,
                            topDownSerialization->OffsetPosition, topDownSerialization->OffsetRotation,
                            topDownSerialization->MaxLookAhead, topDownSerialization->SmoothSpeed,
                            topDownSerialization->MaxMoveSpeed);
                        view->emplace(entity, topDown);
                    } else if (const auto &playerSerialization = dynamic_cast<PlayerControllerComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<PlayerControllerComponent>();
                        PlayerControllerComponent player(playerSerialization->MoveSpeed, playerSerialization->DestinationDistance);
                        view->emplace(entity, player);
                    } else if (const auto &lightSerialization = dynamic_cast<DirectionalLightComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<DirectionalLightComponent>();
                        view->emplace(entity, *lightSerialization);
                    } else if (const auto &healthSerialization = dynamic_cast<HealthComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<HealthComponent>();
                        HealthComponent healthComp(healthSerialization->MaxHealth);
                        view->emplace(entity, healthComp);
                    } else if (const auto &particleSerialization = dynamic_cast<ParticleEmitterComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<ParticleEmitterComponent>();
                        view->emplace(entity, *particleSerialization);
                    } else if (const auto &ikAimSerialization = dynamic_cast<IKAimComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<IKAimComponent>();
                        IKAimComponent ikComp;
                        ikComp.Enabled = ikAimSerialization->Enabled;
                        ikComp.BlendWeight = ikAimSerialization->BlendWeight;
                        ikComp.BlendSpeed = ikAimSerialization->BlendSpeed;
                        ikComp.TorsoMaxYaw = ikAimSerialization->TorsoMaxYaw;
                        ikComp.TorsoMaxPitch = ikAimSerialization->TorsoMaxPitch;
                        ikComp.SpineWeight = ikAimSerialization->SpineWeight;
                        ikComp.Spine1Weight = ikAimSerialization->Spine1Weight;
                        ikComp.ChestWeight = ikAimSerialization->ChestWeight;
                        ikComp.RightHandOffset = ikAimSerialization->RightHandOffset;
                        ikComp.LeftHandOffset = ikAimSerialization->LeftHandOffset;
                        ikComp.LeftHandGripPoint = ikAimSerialization->LeftHandGripPoint;
                        ikComp.RightElbowHint = ikAimSerialization->RightElbowHint;
                        ikComp.LeftElbowHint = ikAimSerialization->LeftElbowHint;
                        ikComp.RotateCharacterToAim = ikAimSerialization->RotateCharacterToAim;
                        ikComp.CharacterRotationSpeed = ikAimSerialization->CharacterRotationSpeed;
                        view->emplace(entity, ikComp);
                    }
                }
            }

        }
    }

    void InitSystems(std::shared_ptr<RenderRepository> &renderRepository, const std::shared_ptr<ResourceCache>& resourceCache, const std::weak_ptr<AudioManager>& audioManager = std::weak_ptr<AudioManager>()) {
        if (const auto &scenePtr = _scene.lock()) {

            const auto registry = scenePtr->GetEntityRegistry();

            const auto &windowView = registry->view<WindowComponent>();
            _systems.emplace_back(std::make_unique<WindowSystem>(windowView, scenePtr->GetWindow()));

            _systems.emplace_back(std::make_unique<DeltaTimeSystem>(registry->view<DeltaTimeComponent>()));

            _systems.emplace_back(std::make_unique<CameraControllerSystem>(registry->view<CameraControllerComponent, TransformComponentV2>(),
                registry->view<DeltaTimeComponent>(), scenePtr));

            _systems.emplace_back(std::make_unique<CameraSystem>(registry->view<WindowComponent>(),registry->view<CameraComponentV2, TransformComponentV2>()));

            _systems.emplace_back(std::make_unique<DirectionalLightSystem>(
                registry->view<DirectionalLightComponent, TransformComponentV2>(),
                registry->view<TagComponent, TransformComponentV2>(),
                renderRepository));

            // Player controller must run BEFORE navmesh agent to set destination
            _systems.emplace_back(std::make_unique<PlayerControllerSystem>(
                registry->view<PlayerControllerComponent, NavmeshAgentComponent, TransformComponentV2>(),
                registry->view<DeltaTimeComponent>(), scenePtr, *registry));

            // Navmesh agent must run BEFORE camera so velocity is updated for look-ahead
            _systems.emplace_back(std::make_unique<NavmeshAgentSystem>(registry->view<NavmeshAgentComponent, TransformComponentV2>(),
               registry->view<DeltaTimeComponent>(), scenePtr->GetNavigationManager(), scenePtr));

            // Top-down camera follows target entity with look-ahead (needs fresh velocity from navmesh agent)
            _systems.emplace_back(std::make_unique<TopDownCameraSystem>(
                registry->view<TopDownCameraComponent, TransformComponentV2>(),
                registry->view<TagComponent, TransformComponentV2>(),
                registry->view<DeltaTimeComponent>(),
                *registry));

            _systems.emplace_back(std::make_unique<MeshRenderSystem>(registry->view<MeshRendererComponent, TransformComponentV2>(),
               registry->view<CameraComponentV2>(), renderRepository, resourceCache));

            // Animation systems must run AFTER navmesh (for velocity) and BEFORE skinned mesh renderer
            _systems.emplace_back(std::make_unique<SimpleAnimationSystem>(registry->view<SimpleAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
               registry->view<DeltaTimeComponent>(), resourceCache));

            _systems.emplace_back(std::make_unique<FsmAnimationSystem>(registry->view<FsmAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
               registry->view<DeltaTimeComponent>(), scenePtr, resourceCache, *registry));

            // IK Aim system - runs AFTER animation, BEFORE skinned mesh render
            _systems.emplace_back(std::make_unique<IKAimSystem>(
                registry->view<IKAimComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
                registry->view<DeltaTimeComponent>(),
                registry->view<CameraComponentV2, TransformComponentV2>(),
                registry->view<WindowComponent>(),
                scenePtr,
                *registry));

            _systems.emplace_back(std::make_unique<SkinnedMeshRenderSystem>(registry->view<SkinnedMeshRendererComponent, TransformComponentV2>(),
               registry->view<CameraComponentV2>(), renderRepository, resourceCache));
#ifndef NDEBUG
            _systems.emplace_back(std::make_unique<NavmeshPathRenderSystem>(registry->view<NavmeshAgentComponent>(),
               registry->view<CameraComponentV2>(), renderRepository, scenePtr->GetNavigationManager()));

            _systems.emplace_back(std::make_unique<NavmeshDebugRenderSystem>(
               registry->view<CameraComponentV2>(), renderRepository, scenePtr->GetNavigationManager()));
#endif
            _systems.emplace_back(std::make_unique<SpawnerSystem>(registry->view<SpawnerComponent>(), scenePtr));

            // Behavior tree system - runs after navigation for velocity data
            _systems.emplace_back(std::make_unique<BehaviorTreeSystem>(
                registry->view<BehaviorTreeComponent>(),
                *registry,
                scenePtr,
                registry->view<DeltaTimeComponent>(),
                registry->view<CameraComponentV2, TransformComponentV2>()));

            // Health system - monitors player health and handles death
            _systems.emplace_back(std::make_unique<HealthSystem>(
                registry->view<HealthComponent, TransformComponentV2>(),
                registry->view<DeltaTimeComponent>(),
                registry->view<BehaviorTreeComponent, TransformComponentV2>()));

            // Audio system - handles spatial audio and auto-play
            _systems.emplace_back(std::make_unique<AudioSystem>(
                registry->view<AudioSourceComponent, TransformComponentV2>(),
                registry->view<AudioListenerComponent, TransformComponentV2>(),
                audioManager));

            // Particle system - updates particle physics and spawning
            _systems.emplace_back(std::make_unique<ParticleSystem>(
                registry->view<ParticleEmitterComponent, TransformComponentV2>(),
                registry->view<DeltaTimeComponent>(),
                registry->view<ParticleEmitterComponent, NavmeshAgentComponent>(),
                registry->view<ParticleEmitterComponent, HealthComponent>(),
                registry->view<ParticleEmitterComponent, BehaviorTreeComponent>()));

            // Particle render system - renders particles
            _systems.emplace_back(std::make_unique<ParticleRenderSystem>(
                registry->view<ParticleEmitterComponent, TransformComponentV2>(),
                registry->view<CameraComponentV2>(),
                renderRepository,
                scenePtr));
        }
    }


    void Update() const {
        PROFILE_SCOPE("ECS::Update");
        for (const auto &system : _systems) {
            system->OnTick();
        }
    }

};
