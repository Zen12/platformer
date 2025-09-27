#pragma once

#include "grid_component.hpp"
#include "grid_component_serialization.hpp"
#include "../../asset/factories/component_factory.hpp"

class GridFactory final : public ComponentFactory<GridComponent, GridSerialization> {
protected:
    void FillComponent(const std::weak_ptr<GridComponent> &component, const GridSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            if (const auto scene = _scene.lock()) {
                comp->SetGrid(serialization.grid);
                comp->SetSpawnOffset(serialization.spawnOffset);
                comp->SetSpawnStep(serialization.spawnStep);
            }
        }
    }
};