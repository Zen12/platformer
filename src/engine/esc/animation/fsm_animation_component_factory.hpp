#pragma once
#include "../../entity/component_factory.hpp"
#include "fsm_animation_component.hpp"
#include "fsm_animation_component_serialization.hpp"

class FsmAnimationComponentFactory final : public ComponentFactory<FsmAnimationComponent, FsmAnimationComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<FsmAnimationComponent> &component, const FsmAnimationComponentSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            comp->FsmGuid = serialization.FsmGuid;
            comp->Loop = serialization.Loop;
            comp->ApplyRootMotion = serialization.ApplyRootMotion;
            comp->RootBoneName = serialization.RootBoneName;
        }
    }
};
