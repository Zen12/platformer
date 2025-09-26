#pragma once

#include "button_component.hpp"
#include "button_component_serialization.hpp"
#include "../../asset/factories/component_factory.hpp"

class UiButtonComponentFactory final : public ComponentFactory<UiButtonComponent, UiButtonComponentSerialization> {
protected:
    void FillComponent([[maybe_unused]] const std::weak_ptr<UiButtonComponent> &component,
        [[maybe_unused]] const UiButtonComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto entity = _entity.lock()) {
                if (const auto comp = component.lock()) {
                    comp->SetRectTransform(entity->GetComponent<RectTransform>());
                    comp->SetSelf(component);
                    comp->SetUiRaycastSystem(scene->GetUiRaycast());
                }
            }
        }
    }
};