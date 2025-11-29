#pragma once

#include "camera/camera_component.hpp"
#include "camera/camera_system.hpp"
#include "entt/entt.hpp"
#include "mesh_renderer/mesh_renderer_component.hpp"
#include "mesh_renderer/mesh_render_system.hpp"
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
        }
    }


    void Update() const {
        for (const auto &system : _systems) {
            system->OnTick();
        }
    }

};