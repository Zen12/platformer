#pragma once
#include "../asset/factories/component_factory.hpp"
#include "astar_finder_component.hpp"
#include "astar_finder_component_serialization.hpp"

class PathFinderFactory final : public ComponentFactory<AStarFinderComponent, PathFinderSerialization> {
protected:
    void FillComponent(const std::weak_ptr<AStarFinderComponent> &component, const PathFinderSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            if (const auto scene = _scene.lock()) {
                const auto entity = scene->FindByTag(serialization.gridTag);
                comp->Initialize(entity.lock()->GetComponent<GridComponent>());
            }
        }
    }
};