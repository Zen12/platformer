#pragma once

#include "destroy_with_creator_component.hpp"
#include "destroy_with_creator_component_serialization.hpp"
#include "../../asset/factories/component_factory.hpp"

class DestroyWithCreatorComponentFactory final : public ComponentFactory<DestroyWithCreatorComponent, DestroyWithCreatorComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<DestroyWithCreatorComponent> &component,
        [[maybe_unused]] const DestroyWithCreatorComponentSerialization &serialization) override {

        if (const auto comp = component.lock()) {
            comp->SetScene(_scene.lock());
        }
    }
};