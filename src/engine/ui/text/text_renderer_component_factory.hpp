#pragma once
#include "text_renderer_component.hpp"
#include "text_renderer_component_serialization.hpp"
#include "../../entity/component_factory.hpp"

class UiTextComponentFactory final : public ComponentFactory<UiTextRendererComponent, UiTextComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<UiTextRendererComponent> &component, const UiTextComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                const auto material = scene->GetMaterial(serialization.MaterialGUID);

                comp->SetMaterial(material);
                comp->SetText(serialization.Text);
                comp->SetColor(serialization.Color);
                comp->SetFontSize(serialization.FontSize);

                scene->GetRenderPipeline().lock()->AddRenderer(comp);
            }
        }
    }
};
