#pragma once

#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include "profiler.hpp"
#include "tag/tag_component.hpp"
#include "time/time_component.hpp"
#include "window/window_component.hpp"
#include "buffer/render_buffer_component.hpp"

#include "entity/component_registry.hpp"
#include "transform/transform_component.hpp"

#include "esc_system_context.hpp"
#include "core/src/register/esc_system_registry.hpp"

class EscSystem {
    const std::weak_ptr<Scene> _scene;
    std::vector<std::unique_ptr<ISystem>> _fixedSystems;
    std::vector<std::unique_ptr<ISystem>> _renderSystems;
    mutable float _accumulator = 0.0f;
    static constexpr float FIXED_TIMESTEP = 0.02f;
    static constexpr float MAX_ACCUMULATOR = 0.1f;
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

    void InitSystems(std::shared_ptr<RenderBuffer> &renderBuffer, const std::shared_ptr<ResourceFactory>& resourceFactory, const std::weak_ptr<AudioManager>& audioManager = std::weak_ptr<AudioManager>(), const EscSystemRegistry* systemRegistry = nullptr, const std::string& projectRoot = "") {
        if (const auto &scenePtr = _scene.lock()) {

            if (systemRegistry) {
                const auto registry = scenePtr->GetEntityRegistry();
                registry->emplace<RenderBufferComponent>(_systemEntity, RenderBufferComponent{renderBuffer});

                EscSystemContext ctx;
                ctx.Registry = registry;
                ctx.Scene = _scene;
                ctx.ResourceFactory = resourceFactory;
                ctx.AudioManager = audioManager;
                ctx.ProjectRoot = projectRoot;

                _fixedSystems = systemRegistry->BuildByPhase(ctx, SystemPhase::FIXED);
                _renderSystems = systemRegistry->BuildByPhase(ctx, SystemPhase::RENDER);
            }
        }
    }


    void Update() const {
        PROFILE_SCOPE("ECS::Update");
        if (const auto &scenePtr = _scene.lock()) {
            const auto registry = scenePtr->GetEntityRegistry();

            float frameDelta = 0.0f;
            for (auto [entity, time] : registry->view<DeltaTimeComponent>().each()) {
                frameDelta = time.Delta;
            }

            _accumulator += frameDelta;
            if (_accumulator > MAX_ACCUMULATOR) {
                _accumulator = MAX_ACCUMULATOR;
            }

            while (_accumulator >= FIXED_TIMESTEP) {
                for (const auto &system : _fixedSystems) {
                    system->OnTick();
                }
                _accumulator -= FIXED_TIMESTEP;
            }
        }

        for (const auto &system : _renderSystems) {
            system->OnTick();
        }
    }

};
