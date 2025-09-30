#pragma once

#include "grid_prefab_spawner_component.hpp"
#include "grid_prefab_spawner_component_serialization.hpp"
#include "../../entity/component_factory.hpp"

class GridPrefabSpawnerFactor final : public ComponentFactory<GridPrefabSpawner, GridPrefabSpawnerSerialization> {
protected:
    void FillComponent(const std::weak_ptr<GridPrefabSpawner> &component, const GridPrefabSpawnerSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            if (const auto scene = _scene.lock()) {
                comp->SetPrefabId(serialization.prefabId);
                comp->SetScene(_scene);
                comp->Spawn(scene->FindByTag(serialization.gridTag).lock()->GetComponent<GridComponent>());
            }
        }
    }
};