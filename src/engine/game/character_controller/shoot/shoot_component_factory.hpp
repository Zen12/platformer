#pragma once
#include "../../../entity/component_factory.hpp"
#include "shoot_component.hpp"
#include "shoot_component_serialization.hpp"

class ShootComponentFactory final : public ComponentFactory<ShootComponent, ShootComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<ShootComponent> &component, const ShootComponentSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                if (const auto entity = _entity.lock()) {
                    comp->SetWorld(scene->GetPhysicsWorld());
                    comp->SetAnimation(entity->GetComponent<CharacterAnimationComponent>());
                    comp->SetBoneName(serialization.BoneName);
                    comp->SetRaycastDistance(serialization.RaycastDistance);
                }
            }
        }
    }
};
