#pragma once
#include "entity/component_factory.hpp"
#include "spawner_component.hpp"
#include "spawner_component_serialization.hpp"

class SpawnerComponentFactory final : public ComponentFactory<SpawnerComponent, SpawnerComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<SpawnerComponent> &component, const SpawnerComponentSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            // Component is initialized with constructor parameters
            // No additional setup needed
        }
    }
};
