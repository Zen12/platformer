#pragma once
#include "../../asset/factories/component_factory.hpp"
#include "spine_collider_component.hpp"
#include "spine_collider_component_serialization.hpp"

class SpineColliderComponentFactory final : public ComponentFactory<SpineColliderComponent, SpineColliderSerialization> {
protected:
    void FillComponent(const std::weak_ptr<SpineColliderComponent> &component,
        [[maybe_unused]] const SpineColliderSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto entity = _entity.lock()) {
                if (const auto comp = component.lock()) {
                    comp->SetPhysicsWorld(scene->GetPhysicsWorld());
                    comp->SetRenderer(entity->GetComponent<SpineRenderer>());
                }
            }
        }
    }
};