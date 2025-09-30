#pragma once
#include "../../entity/component_factory.hpp"
#include "health_component.hpp"
#include "health_component_serialization.hpp"

class HealthComponentFactory final : public ComponentFactory<HealthComponent, HealthComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<HealthComponent> &component, const HealthComponentSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            comp->SetMaxHealth(serialization.Health);
            comp->SetHealth(serialization.Health);
            comp->SetScene(_scene.lock());
        }
    }
};