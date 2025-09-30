#pragma once
#include "../entity/component_factory.hpp"
#include "path_mover_component.hpp"
#include "path_mover_component_serialization.hpp"

class PathMoverComponentFactor final : public ComponentFactory<PathMover, PathMoverComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<PathMover> &component, const PathMoverComponentSerialization &serialization) override {

        if (const auto entity = _entity.lock()) {
            if (const auto scene = _scene.lock()) {
                if (const auto comp = component.lock()) {

                    comp->SetTransform(entity->GetComponent<Transform>());
                    comp->SetPoints(serialization.Positions);
                    comp->SetSpeed(serialization.Speed);
                }
            }
        }
    }
};