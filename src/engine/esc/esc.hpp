#pragma once

#include "animation/simple_animation_component.hpp"
#include "animation/simple_animation_component_serialization.hpp"
#include "animation/simple_animation_system.hpp"
#include "ozz_animation/ozz_animation_component.hpp"
#include "ozz_animation/ozz_animation_component_serialization.hpp"
#include "ozz_animation/ozz_skeleton_component.hpp"
#include "ozz_animation/ozz_skeleton_component_serialization.hpp"
#include "ozz_animation/ozz_ik_component.hpp"
#include "ozz_animation/ozz_ik_component_serialization.hpp"
#include "ozz_animation/mouse_ik_controller_component.hpp"
#include "ozz_animation/mouse_ik_controller_component_serialization.hpp"
#include "ozz_animation/ozz_animation_system.hpp"
#include "ozz_animation/mouse_ik_controller_system.hpp"
#include "camera/camera_component.hpp"
#include "camera/camera_system.hpp"
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
                        view->emplace(entity, SimpleAnimationComponent(animationSerialization->AnimationGuid));
                    } else if (const auto &ozzAnimSerialization = dynamic_cast<OzzAnimationComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<OzzAnimationComponent>();
                        OzzAnimationComponent comp(ozzAnimSerialization->AnimationGuid);
                        comp.PlaybackSpeed = ozzAnimSerialization->PlaybackSpeed;
                        comp.Loop = ozzAnimSerialization->Loop;
                        comp.Playing = ozzAnimSerialization->Playing;
                        view->emplace(entity, comp);
                    } else if (const auto &ozzSkelSerialization = dynamic_cast<OzzSkeletonComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<OzzSkeletonComponent>();
                        view->emplace(entity, OzzSkeletonComponent(ozzSkelSerialization->SkeletonGuid));
                    } else if (const auto &ozzIkSerialization = dynamic_cast<OzzIKComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<OzzIKComponent>();
                        OzzIKComponent comp;
                        comp.Type = ozzIkSerialization->Type == "two_bone" ? IKType::TwoBone : IKType::Aim;
                        comp.Enabled = ozzIkSerialization->Enabled;
                        comp.Weight = ozzIkSerialization->Weight;
                        comp.StartBoneName = ozzIkSerialization->StartBoneName;
                        comp.MidBoneName = ozzIkSerialization->MidBoneName;
                        comp.EndBoneName = ozzIkSerialization->EndBoneName;
                        comp.Target = ozzIkSerialization->Target;
                        comp.PoleVector = ozzIkSerialization->PoleVector;
                        comp.AimAxis = ozzIkSerialization->AimAxis;
                        comp.UpAxis = ozzIkSerialization->UpAxis;
                        view->emplace(entity, comp);
                    } else if (const auto &mouseIkSerialization = dynamic_cast<MouseIKControllerComponentSerialization*>(component.get())) {
                        const auto &view = registry->view<MouseIKControllerComponent>();
                        MouseIKControllerComponent comp;
                        comp.Enabled = mouseIkSerialization->Enabled;
                        comp.PlaneNormal = mouseIkSerialization->PlaneNormal;
                        comp.PlaneDistance = mouseIkSerialization->PlaneDistance;
                        comp.TargetOffset = mouseIkSerialization->TargetOffset;
                        view->emplace(entity, comp);
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

            _systems.emplace_back(std::make_unique<CameraSystem>(registry->view<WindowComponent>(),registry->view<CameraComponentV2, TransformComponentV2>()));

            _systems.emplace_back(std::make_unique<MeshRenderSystem>(registry->view<MeshRendererComponent, TransformComponentV2>(),
               registry->view<CameraComponentV2>(), renderRepository ));

            // Animation systems must run BEFORE skinned mesh renderer to compute bone transforms
            _systems.emplace_back(std::make_unique<SimpleAnimationSystem>(registry->view<SimpleAnimationComponent, SkinnedMeshRendererComponent>(),
               registry->view<DeltaTimeComponent>(), scenePtr));

            // Mouse IK controller must run BEFORE animation system to update IK targets
            if (auto inputSystem = scenePtr->GetInputSystem().lock()) {
                _systems.emplace_back(std::make_unique<MouseIKControllerSystem>(
                    registry->view<MouseIKControllerComponent, OzzIKComponent>(),
                    inputSystem));
            }

            _systems.emplace_back(std::make_unique<OzzAnimationSystem>(registry->view<OzzAnimationComponent, OzzSkeletonComponent, SkinnedMeshRendererComponent>(),
               registry->view<DeltaTimeComponent>(), scenePtr));

            _systems.emplace_back(std::make_unique<SkinnedMeshRenderSystem>(registry->view<SkinnedMeshRendererComponent, TransformComponentV2>(),
               registry->view<CameraComponentV2>(), renderRepository ));
        }
    }


    void Update() const {
        for (const auto &system : _systems) {
            system->OnTick();
        }
    }

};