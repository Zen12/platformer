#pragma once

#include "image_renderer_component.hpp"
#include "image_renderer_component_serialization.hpp"
#include "../../entity/component_factory.hpp"

class UiImageFactory final : public ComponentFactory<UiImageRendererComponent, UiImageComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<UiImageRendererComponent> &component, const UiImageComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto uiImage = component.lock()) {
                const auto material = scene->GetMaterial(serialization.MaterialGuid);
                const auto sprite = scene->GetSprite(serialization.SpriteGuid);

                uiImage->SetMaterial(material);
                uiImage->SetSprite(sprite);
                uiImage->SetFillAmount(serialization.FillAmount);

                scene->GetRenderPipeline().lock()->AddRenderer(component);
            }
        }
    }
};