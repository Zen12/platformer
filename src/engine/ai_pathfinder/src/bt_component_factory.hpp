#pragma once
#include "../../entity/component_factory.hpp"
#include "bt_component.hpp"
#include "bt_component_serialization.hpp"

class BehaviorTreeComponentFactory final : public ComponentFactory<BehaviorTreeComponent, BehaviorTreeComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<BehaviorTreeComponent>& component, const BehaviorTreeComponentSerialization& serialization) override {
        if (const auto comp = component.lock()) {
            comp->TreeGuid = serialization.TreeGuid;
            comp->TickPriority = serialization.TickPriority;
            // TreeDef will be loaded by BehaviorTreeSystem using TreeGuid
        }
    }
};
