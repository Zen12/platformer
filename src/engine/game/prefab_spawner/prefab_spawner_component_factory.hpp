#pragma once

#include "prefab_spawner_component.hpp"
#include "prefab_spawner_component_serialization.hpp"
#include "../../entity/component_factory.hpp"

class PrefabSpawnerFactor final : public ComponentFactory<PrefabSpawner, PrefabSpawnerSerialization> {
protected:
    void FillComponent(const std::weak_ptr<PrefabSpawner> &component, const PrefabSpawnerSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            comp->SetPrefabId(serialization.prefabId);
            comp->SetScene(_scene);
            comp->SetMaxCount(serialization.maxSpawn);
            comp->SetSpawnTimer(serialization.spawnTime);
            comp->SetPrefabPositions(serialization.positions);
        }
    }
};