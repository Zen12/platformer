#pragma once

#include "health_bar_component.hpp"
#include "health_bar_component_serialization.hpp"
#include "../health_component.hpp"
#include "../../components/ui/ui_image_renderer.hpp"

template<typename TComponent, typename TSerialization>
class ComponentFactory;

class HealthBarComponentFactory final : public ComponentFactory<HealthBarComponent, HealthBarComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<HealthBarComponent> &component,
        const HealthBarComponentSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto entity = _entity.lock()) {
                if (const auto comp = component.lock()) {
                    const auto images = entity->GetComponents<UiImageRenderer>();
                    comp->SetFillBar(images.back());

                    if (serialization.UseCreator) {
                        if (const auto creator = entity->GetCreator(); !creator.empty()) {
                            const auto creatorEntity = scene->GetEntityById(creator);
                            if (const auto e = creatorEntity.lock()) {
                                comp->SetHealth(e->GetComponent<HealthComponent>());
                            }
                        }
                    } else {
                        comp->SetHealth(entity->GetComponent<HealthComponent>());
                    }
                }
            }
        }
    }
};