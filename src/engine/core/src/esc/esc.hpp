#pragma once

#include "profiler.hpp"
#include "tag/tag_component.hpp"
#include "time/time_component.hpp"
#include "window/window_component.hpp"
#include "buffer/render_buffer_component.hpp"

#include "entity/component_registry.hpp"

#include "esc_system_context.hpp"
#include "core/src/register/esc_system_registry.hpp"

class EscSystem {
    const std::weak_ptr<Scene> _scene;
    std::vector<std::unique_ptr<ISystem>> _systems;
    const ComponentRegistry* _componentRegistry = nullptr;
    entt::entity _systemEntity = entt::null;

public:
    explicit EscSystem(const std::weak_ptr<Scene> &scene, const ComponentRegistry* componentRegistry = nullptr)
    : _scene(scene), _componentRegistry(componentRegistry) {

        if (const auto &scenePtr = _scene.lock()) {

            const auto registry = scenePtr->GetEntityRegistry();
            _systemEntity = registry->create();

            registry->view<WindowComponent>()->emplace(_systemEntity, WindowComponent());
            registry->view<DeltaTimeComponent>()->emplace(_systemEntity, DeltaTimeComponent());
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
                    if (_componentRegistry && !component->ComponentType.empty()) {
                        _componentRegistry->Emplace(component->ComponentType, *registry, entity, component.get());
                    }
                }
            }
        }
    }

    void InitSystems(std::shared_ptr<RenderBuffer> &renderBuffer, const std::shared_ptr<ResourceCache>& resourceCache, const std::weak_ptr<AudioManager>& audioManager = std::weak_ptr<AudioManager>(), const EscSystemRegistry* systemRegistry = nullptr) {
        if (const auto &scenePtr = _scene.lock()) {

            if (systemRegistry) {
                const auto registry = scenePtr->GetEntityRegistry();
                registry->emplace<RenderBufferComponent>(_systemEntity, RenderBufferComponent{renderBuffer});

                EscSystemContext ctx;
                ctx.Registry = registry;
                ctx.Scene = _scene;
                ctx.ResourceCache = resourceCache;
                ctx.AudioManager = audioManager;

                _systems = systemRegistry->BuildAll(ctx);
            }
        }
    }


    void Update() const {
        PROFILE_SCOPE("ECS::Update");
        for (const auto &system : _systems) {
            system->OnTick();
        }
    }

};
