#pragma once

#include "health_bar_component.hpp"
#include "health_bar_component_serialization.hpp"
#include "../../game/health/health_component.hpp"
#include "../../ui/image/image_renderer_component.hpp"

class HealthBarComponentFactory {
private:
    entt::registry &_registry;

public:
    explicit HealthBarComponentFactory(entt::registry &registry)
        : _registry(registry)
    {}

    void FillComponent(const entt::entity &e, const HealthBarComponentSerialization &serialization, std::weak_ptr<Scene> _scene) const {

        const auto viewEntity = _registry.view<std::shared_ptr<Entity>>();
        const auto viewHealth = _registry.view<HealthComponent>();
        const auto _entity = viewEntity.get<std::shared_ptr<Entity>>(e);


        if (const auto scene = _scene.lock()) {
            if (const auto& entity = _entity) {
                const auto images = entity->GetComponents<UiImageRendererComponent>();

                if (serialization.UseCreator) {
                    if (const auto creator = entity->GetCreator(); !creator.empty()) {
                        const auto creatorEntity = scene->GetEntityById(creator);
                        if (const auto eParent = creatorEntity.lock()) {
                            for (auto entityParent : viewEntity) {
                                if (viewEntity.get<std::shared_ptr<Entity>>(entityParent).get() == eParent.get()) {
                                    HealthComponent *healthComponent = &viewHealth.get<HealthComponent>(entityParent);
                                    UiImageRendererComponent *render = eParent->GetComponent<UiImageRendererComponent>().lock().get();
                                    _registry.emplace<HealthBarComponent>(e, healthComponent, render);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};