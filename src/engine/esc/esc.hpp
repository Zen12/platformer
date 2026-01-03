#pragma once

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
#include "navmesh_agent/random_navigation_component.hpp"
#include "navmesh_agent/random_navigation_system.hpp"
#include "navmesh_path_renderer/navmesh_path_render_system.hpp"
#include "navmesh_debug_renderer/navmesh_debug_render_system.hpp"
#include "spawner/spawner_component.hpp"
#include "spawner/spawner_component_serialization.hpp"
#include "spawner/spawner_system.hpp"


class EscSystem {
    const std::weak_ptr<Scene> _scene;
    std::vector<std::unique_ptr<ISystem>> _systems;

public:
    explicit EscSystem(const std::weak_ptr<Scene> &scene)
    : _scene(scene) {

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

                        const auto &randomNavView = registry->view<RandomNavigationComponent>();
                        randomNavView->emplace(entity, RandomNavigationComponent());
                    } else if (const auto &spawnerSerialization = dynamic_cast<SpawnerComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<SpawnerComponent>();
                        SpawnerComponent spawner(spawnerSerialization->PrefabGuid, spawnerSerialization->SpawnCount, spawnerSerialization->SpawnPositions, spawnerSerialization->SpawnOnNavmesh, spawnerSerialization->SpawnOnAllCells);
                        view->emplace(entity, spawner);
                    }
                }
            }

        }
    }

    void InitSystems(std::shared_ptr<RenderRepository> &renderRepository) {
        if (const auto &scenePtr = _scene.lock()) {

            const auto registry = scenePtr->GetEntityRegistry();

            const auto &windowView = registry->view<WindowComponent>();
            _systems.emplace_back(std::make_unique<WindowSystem>(windowView, scenePtr->GetWindow()));

            _systems.emplace_back(std::make_unique<DeltaTimeSystem>(registry->view<DeltaTimeComponent>()));

            _systems.emplace_back(std::make_unique<CameraControllerSystem>(registry->view<CameraControllerComponent, TransformComponentV2>(),
                registry->view<DeltaTimeComponent>(), scenePtr));

            _systems.emplace_back(std::make_unique<CameraSystem>(registry->view<WindowComponent>(),registry->view<CameraComponentV2, TransformComponentV2>()));

            _systems.emplace_back(std::make_unique<MeshRenderSystem>(registry->view<MeshRendererComponent, TransformComponentV2>(),
               registry->view<CameraComponentV2>(), renderRepository ));

            // Animation systems must run BEFORE skinned mesh renderer to compute bone transforms
            _systems.emplace_back(std::make_unique<SimpleAnimationSystem>(registry->view<SimpleAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
               registry->view<DeltaTimeComponent>(), scenePtr));

            _systems.emplace_back(std::make_unique<FsmAnimationSystem>(registry->view<FsmAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2>(),
               registry->view<DeltaTimeComponent>(), scenePtr));

            _systems.emplace_back(std::make_unique<SkinnedMeshRenderSystem>(registry->view<SkinnedMeshRendererComponent, TransformComponentV2>(),
               registry->view<CameraComponentV2>(), renderRepository ));

            _systems.emplace_back(std::make_unique<RandomNavigationSystem>(registry->view<RandomNavigationComponent, NavmeshAgentComponent, TransformComponentV2>(),
               registry->view<DeltaTimeComponent>(), scenePtr->GetNavigationManager()));

            _systems.emplace_back(std::make_unique<NavmeshAgentSystem>(registry->view<NavmeshAgentComponent, TransformComponentV2>(),
               registry->view<DeltaTimeComponent>(), scenePtr->GetNavigationManager(), scenePtr));
#ifndef NDEBUG
            _systems.emplace_back(std::make_unique<NavmeshPathRenderSystem>(registry->view<NavmeshAgentComponent>(),
               registry->view<CameraComponentV2>(), renderRepository, scenePtr->GetNavigationManager()));

            _systems.emplace_back(std::make_unique<NavmeshDebugRenderSystem>(
               registry->view<CameraComponentV2>(), renderRepository, scenePtr->GetNavigationManager()));
#endif
            _systems.emplace_back(std::make_unique<SpawnerSystem>(registry->view<SpawnerComponent>(), scenePtr));
        }
    }


    void Update() const {
        for (const auto &system : _systems) {
            system->OnTick();
        }
    }

};