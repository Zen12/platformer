#pragma once
#include "../../entity/component_factory.hpp"
#include "transform_component.hpp"
#include "transform_component_serialization.hpp"

class TransformFactory final : public ComponentFactory<Transform, TransformComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<Transform> &component, const TransformComponentSerialization &serialization) override {
        if (const auto transform = component.lock()) {
            transform->SetPosition(serialization.position);
            transform->SetEulerRotation(serialization.rotation);
            transform->SetScale(serialization.scale);
        }
    }
};