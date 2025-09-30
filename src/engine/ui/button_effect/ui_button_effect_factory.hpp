#pragma once

#include "ui_button_effect_serialization.hpp"
#include "../../entity/component_factory.hpp"
#include "ui_button_effect.hpp"
#include "../button/button_component.hpp"
#include "../image/image_renderer_component.hpp"

class UiButtonEffectFactory final : public ComponentFactory<UiButtonEffect, UiButtonEffectSerialization> {
protected:
    void FillComponent(const std::weak_ptr<UiButtonEffect> &component,
        [[maybe_unused]] const UiButtonEffectSerialization &serialization) override
    {
        if (const auto entity = _entity.lock()) {
            if (const auto &comp = component.lock()) {
                comp->SetButton(entity->GetComponent<UiButtonComponent>());
                comp->SetTarget(entity->GetComponent<UiImageRendererComponent>());
            }
        }
    }
};